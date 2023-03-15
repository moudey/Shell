Option Strict On

Imports System.IO
Imports System.Globalization
Imports System.Text.Json
Imports System.XML
Imports System.XML.Linq

Partial Class _Default

Inherits System.Web.UI.Page

Friend Shared   SERVER_NAME     As String
Friend Shared   SERVER_PORT     As String
Friend Shared   HTTPS           As String

Friend Shared   rootUrl         As String       = "http://" & SERVER_NAME
Friend const    nilesoftRoot    As String       = "https://nilesoft.org"
Friend const    gitRepo         As String       = "https://github.com/moudey/shell"
Friend const    gitBranch       As String       = "main"
Friend const    gitRootUrl      As String       = gitRepo & "/blob/" & "main"

Friend Shared   nilesoftUrl     As String
Friend Shared   gitUrl          As String

Friend Shared   appRoot         As DirectoryInfo
Friend Shared   projectRoot     As DirectoryInfo
Friend Shared   docsRoot        As DirectoryInfo

Friend Shared   requestType     As String
Friend Shared   requestUrl      As String
Friend Shared   requestMap      As String
Friend Shared   requestedFile   As FileInfo
Friend Shared   fileModified    As DateTime
Friend Shared   fileUrl         As String

Private Shared  rgxRemoveIndex  As New Regex("(.*)(index)?\.html$")
Private Shared  rgxReplaceDash  As New Regex("[-_]+")

''' Creates a TextInfo based on the "en-US" culture.
Private Shared  myTI            As TextInfo     = New CultureInfo("en-US", False).TextInfo


Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

    ''' Get directories
    appRoot             = New DirectoryInfo(HttpRuntime.AppDomainAppPath)
    projectRoot         = appRoot.parent
    docsRoot            = New DirectoryInfo(projectRoot.FullName & "/docs")

    ''' Get the current requested URL
    requestType          = Request.QueryString("type")

    ''' sanitize for empty values and the default file
    if String.IsNullOrEmpty(requestType) then
        requestType = "document"
    end if

    ''' Get the current requested URL
    requestUrl          = Request.QueryString("path")

    ''' sanitize for empty values and the default file
    if String.IsNullOrEmpty(requestUrl) then
        requestUrl = "/"
    else if requestUrl = "default.aspx" then
        requestUrl = "/"
    end if

    ''' Evaluate which file is required:
    '''

    ''' First, take url at face value
    requestMap    = projectRoot.FullName & requestUrl.replace("/", "\")
    requestedFile = New FileInfo(requestMap)

 End Sub


''' this is called from Default.aspx to actually include the requested content
Protected function preparePage(pageFile As FileInfo, ByRef basename As String, ByRef caption As String) As FileInfo

    Dim dirInfo         As DirectoryInfo
    Dim suffix          = ""
    Dim isHTTPS         = false

     ''' Get server information
     SERVER_NAME         =  Request.ServerVariables.GetValues( "SERVER_NAME")(0)
     SERVER_PORT         =  Request.ServerVariables.GetValues( "SERVER_PORT")(0)
     HTTPS               =  Request.ServerVariables.GetValues( "HTTPS")(0)
     isHTTPS             =  if(HTTPS = "ON", true, false )
 
     ''' Build foot URL
     IF isHTTPS then
         rootUrl         = "https://" & SERVER_NAME & if(SERVER_PORT = "443", "", ":" & SERVER_PORT)
     else
         rootUrl         = "http://" & SERVER_NAME & if(SERVER_PORT = "80", "", ":" & SERVER_PORT)
     end if
 
    ''' If the url ends with ".html", we just go for it
    if Not pageFile.Extension.ToLower() = ".html" Then

        ''' Otherwise try adding ".html" - This would be the case for a file in a directory
        ''' E.g. /docs/installation should really load /docs/installation.html
        suffix          = ".html"
        dirInfo         = New DirectoryInfo(pageFile.FullName)
        pageFile        = New FileInfo(pageFile.FullName & suffix)

        ''' test, if file exists
        if not pageFile.Exists Then

            ''' otherwise it could be a directory, and we look for the "index.html"
            ''' So test for the directory
            if dirInfo.Exists Then
                
                ''' If found, assume the default: index.html
                suffix  = "index.html"
                pageFile    = New FileInfo(dirInfo.FullName & "\" & suffix)
                
            end if

        end if

    end if

    ''' Create helper urls for the bottom of the page
    gitUrl      = gitRootUrl & requestUrl & suffix
    fileUrl     = rgxRemoveIndex.Replace(rootUrl & requestUrl, "$1")
    nilesoftUrl = rgxRemoveIndex.Replace(nilesoftRoot & requestUrl, "$1")

    if pageFile.Exists Then
        fileModified = File.GetLastWriteTime(pageFile.FullName)
    end if

    extractNames(pageFile.Name, basename, caption)

    return pageFile

end function


 ''' this is called from Default.aspx to actually include the requested content
Protected function includePage(pageFile As FileInfo) As Boolean

    if Not pageFile.Exists Then
        return false
    end if

    ' Open the stream and read it back.
    Dim sr As StreamReader = pageFile.OpenText()

    Response.Write( sr.ReadToEnd() )
    sr.Close()
    
    return true

End function

Protected  Function includeImage(imageFile As FileInfo) As Boolean

    if not imageFile.Exists Then

        Response.Clear()
        Response.StatusCode = 404
        Response.StatusDescription = "File not found"
        Response.Flush()

        return false

    end  if

        ''' Set the page's content type ...
    Select Case imageFile.Extension.ToLower()

    Case "png"
    Response.ContentType = "image/png"

    Case "jpg", "jpeg"
    Response.ContentType = "image/jpeg"

    Case Else
    Response.ContentType = "application/octet-stream"

    End Select

    Response.TransmitFile(imageFile.FullName)

    Response.Flush()

    return true
    
End Function

''' This is used to automagically create the left-side navigation. It's not perfect, but does the job
Protected function PrintNav(ByVal initial As DirectoryInfo) As XElement

    ''' create return element
    Dim xmlRoot         As XElement         = <html/>
    Dim children        As IEnumerable(Of XElement)

    Dim menuFile        As FileInfo         = New FileInfo(initial.FullName & "\menu.json")


    If menuFile.Exists Then
        PrintNavXmlAppendChildren(xmlRoot, PrintNavJson(menuFile))
    Else
        PrintNavXmlAppendChildren(xmlRoot, PrintNavFS(initial))
    End If

    return xmlRoot

End Function

Protected Function PrintNavXmlMarkActive(ByRef html As XElement, ByVal link As String) As XElement

    ''' If it's the current page, mark it active
    if link = "/" & requestUrl then
        html.Descendants("a")(0).Attribute("class").Value = "is-active"
    end if
    
    return html
    
End Function

Protected Function PrintNavXmlCreateDir(ByVal caption As String, ByVal link As String) As XElement

    Dim html            As XElement

   ''' Create the html
    html = _
    <li>
        <a href="#" class="" >
            <div class="icon-text">
                <span class="icon"><i class="i i-chevron-down i-lg"></i></span>
                <span class="ml-2"><%= caption %></span>
            </div>
        </a>
    </li>

    ''' Update the href attribute
    html.Descendants("a")(0).Attribute("href").Value = link

    return PrintNavXmlMarkActive(html, link)

End Function

Protected function PrintNavXmlCreateFile(ByVal caption As String, ByVal link As String) As XElement

    Dim html            As XElement

    ''' create the required html
    html = _
    <li>
        <a href="#" class="" >
            <span class="ml-5"><%= caption %></span>
        </a>
    </li>

    ''' update the href property
    html.Descendants("a")(0).Attribute("href").Value = link

    return PrintNavXmlMarkActive(html, link)

End Function

Protected function PrintNavXmlAppendChildren(
    ByRef parentNode As XElement,
    ByRef childNode As XElement
    ) As Long

    Dim children        As IEnumerable(Of XElement)

    ''' get files and sub-folders
    children = childNode.Elements
    
    ''' if found any, append them
    if children.count() > 0 then
    
        parentNode.add( <ul/> )
    
        parentNode.Descendants("ul")(0).add( children )
    
    end if
    
    return children.count()
    
End Function

Protected function PrintNavJson(ByVal menuFile As FileInfo) As XElement

    Dim jsonString = File.ReadAllText(menuFile.FullName)

    Using document As JsonDocument = JsonDocument.Parse(jsonString)

        Dim root As JsonElement = document.RootElement

        return PrintNavJsonArray(root)

    End Using

End Function

Protected function PrintNavJsonArray(items As JsonElement) AS XElement

    Dim xmlRoot         As XElement         = <html/>
    Dim count                               = items.GetArrayLength()

    For Each item As JsonElement In items.EnumerateArray()

        ''' Append to the result
        xmlRoot.Add(PrintNavJsonElement(item))

    Next

    return xmlRoot

End Function

Protected function PrintNavJsonElement(item As JsonElement) AS XElement

    Dim element         As JsonElement = Nothing
    Dim title           As String = "N/A"
    Dim link            As String = "#"
    Dim html            As XElement
    Dim children        As IEnumerable(Of XElement)

    If item.TryGetProperty("title", element) Then
        title = element.GetString()
    End If

    If item.TryGetProperty("link", element) Then
        link = element.GetString()
    End If


    If item.TryGetProperty("items", element) Then

        html = PrintNavXmlCreateDir(title, link)

        PrintNavXmlAppendChildren(html, PrintNavJsonArray(element))

    Else

        html = PrintNavXmlCreateFile(title, link)

    End If

    return html

End Function


''' This is used to automagically create the left-side navigation. It's not perfect, but does the job
Protected function PrintNavFS(ByVal initial As DirectoryInfo) As XElement

    Dim fileName        As FileInfo
    Dim directoryNae    As String           = initial.FullName
    Dim base            As String           = initial.FullName.replace(projectRoot.FullName & "\", "/").replace("\", "/")
    Dim html            As XElement
    Dim children        As IEnumerable(Of XElement)
    Dim basename        As String
    Dim caption         As String
    Dim link            As String

    ''' create return element
    Dim xmlRoot         As XElement         = <html/>

    ''' loop through the files in the given directory
    For Each fileName In initial.GetFiles()

        ''' ignore the index.html, As it was already added As a directory
        if fileName.Name = "index.html" then
            continue for
        end if

        ''' Get the file's basename and convert it from "snake-case" to "Title Case"
        ''' (basename and caption are passed ByRef)
        extractNames(fileName.Name, basename, caption)
        
        ''' create the link
        link = base & "/" & basename

        ''' create the required html
        html = PrintNavXmlCreateFile(caption, link)

        ''' Append to the result
        xmlRoot.Add(html)
    next



    ''' Loop through the sub-directories
    For Each directoryName In initial.GetDirectories()

            ''' Get the file's basename and convert it from "snake-case" to "Title Case"
            ''' (basename and caption are passed ByRef)
            extractNames(directoryName.Name, basename, caption)

            ''' Skip listing all images
            if basename = "images" then
                continue for
            end if

            ''' create the link
            link =  base & "/" & basename

            ''' Create the html
            html =  PrintNavXmlMarkActive(PrintNavXmlCreateDir(caption, link), link)

            ''' get files and sub-folders
            PrintNavXmlAppendChildren(html, PrintNav(directoryName))

            ''' add directory to the result
            xmlRoot.Add(html)
        Next

        return xmlRoot

End function

protected sub extractNames(ByVal path As String, ByRef basename As String, ByRef caption As String)

    basename = rgxRemoveIndex.Replace(path, "$1")
    caption  = myTI.ToTitleCase(rgxReplaceDash.Replace(basename, " "))

end sub


End Class


