<%@ Page Language="VB" AutoEventWireup="false" Codebehind="Default.aspx.vb" CodeFile="Default.aspx.vb" Inherits="_Default" Debug="true" %><%

Select Case requestType

Case "document"

%><!-- #include file = "_page.aspx" --><%

Case "image"

    includeImage(requestedFile)

Case Else

%>
Unknown Type <%= requestType %>
<%

End Select

%>