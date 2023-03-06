Option Strict On

Imports System.IO
Imports System.Globalization
Imports System.XML
Imports System.XML.Linq

Partial Class _Default

Inherits System.Web.UI.Page

Friend Shared   SERVER_NAME     As String
Friend Shared   SERVER_PORT     As String
Friend Shared   HTTPS           As String

Friend Shared   rootUrl         As String       = "http://" & SERVER_NAME
Friend const    gitRepo         As String       = "https://github.com/moudey/shell"
Friend const    gitBranch       As String       = "main"
Friend const    gitRootUrl      As String       = gitRepo & "/blob/" & "main"

Friend Shared   gitUrl          As String

Friend Shared   appRoot         As DirectoryInfo
Friend Shared   projectRoot     As DirectoryInfo
Friend Shared   docsRoot        As DirectoryInfo
Friend Shared   imagesRoot      As DirectoryInfo

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
    imagesRoot          = New DirectoryInfo(docsRoot.FullName & "/images")

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
Protected function includePage(pageFile As FileInfo) As Boolean

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
    if Not pageFile.Extension = "html" Then

        ''' Otherwise try adding ".html" - This would be the case for a file in a directory
        ''' E.g. /docs/installation should really load /docs/installation.html
        suffix          = ".html"
        pageFile        = New FileInfo(pageFile.FullName & suffix)

        ''' test, if exists
        if not pageFile.Exists Then

            ''' otherwise it could be a directory, and we look for the "index.html"

            ''' So test for the directory
            dirInfo = New DirectoryInfo(pageFile.FullName)

            if dirInfo.Exists Then
                
                ''' If found, assume the default: index.html
                suffix  = "index.html"
                pageFile    = New FileInfo(pageFile.FullName & "\" & suffix)
                
            end if

        end if

    end if

    ''' Create helper urls for the bottom of the page
    gitUrl  = gitRootUrl & requestUrl & suffix
    fileUrl = rgxRemoveIndex.Replace(rootUrl & requestUrl, "$1")

    if pageFile.Exists Then
        fileModified = File.GetLastWriteTime(pageFile.FullName)
    else
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
        response.StatusCode = 404
        response.StatusDescription = "File not found"
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
        html = _
        <li>
            <a href="#" class="" >
                <span class="ml-5"><%= caption %></span>
            </a>
        </li>

        ''' update the href property
        html.Descendants("a")(0).Attribute("href").Value = link

        ''' check if it's the currently requested URL
        if link = "/" & requestUrl then
            html.Descendants("a")(0).Attribute("class").Value = "is-active"
        end if

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
            link = base & "/" & basename
            html.Descendants("a")(0).Attribute("href").Value = link

            ''' If it's the current page, mark it active
            if link = "/" & requestUrl then
                html.Descendants("a")(0).Attribute("class").Value = "is-active"
            end if

            ''' get files and sub-folders
            children = PrintNav(directoryName).Elements

            ''' if found any, append them
            if children.count() > 0 then

                html.add( <ul/> )

                html.Descendants("ul")(0).add( children )

            end if

            ''' add directory to the result
            xmlRoot.Add(html)
        Next

        return xmlRoot

End function

private sub extractNames(ByVal path As String, ByRef basename As String, ByRef caption As String)

    basename = rgxRemoveIndex.Replace(path, "$1")
    caption  = myTI.ToTitleCase(rgxReplaceDash.Replace(basename, " "))

end sub


End Class


