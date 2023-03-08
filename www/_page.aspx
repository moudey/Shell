<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <meta
      name="keywords"
      content="nilesoft-shell,context-menu,right-click,shell-extension,file-explorer"
    />
    <meta
      name="description"
      content="Shell is a powerful context menu customizer with highly responsive for Windows File Explorer."
    />
    <meta property="og:type" content="website" />
    <meta property="og:locale" content="en_US" />
    <meta property="og:site_name" content="Nilesoft" />
    <meta property="og:title" content="<%= pageName %> - Shell" />
    <meta
      property="og:description"
      content="Shell is a powerful context menu customizer with highly responsive for Windows File Explorer."
    />
    <meta property="og:url" content="<%= fileUrl %>" />
    <meta property="og:image" content="<%= rootUrl %>/images/logo-512.png" />
    <meta property="og:image:type" content="image/png" />
    <meta name="twitter:site" content="@moudey" />
    <meta name="twitter:creator" content="@moudey" />
    <meta name="twitter:title" content="<%= pageName %> - Shell" />
    <meta
      name="twitter:description"
      content="Shell is a powerful context menu customizer with highly responsive for Windows File Explorer."
    />
    <meta name="twitter:card" content="summary" />
    <meta
      name="twitter:image"
      content="https://nilesoft.org/images/logo-512.png"
    />

    <title><%= pageName %> - Shell</title>
    <link rel="icon" type="image/svg+xml" href="/images/favicon.svg" />
    <link rel="alternate icon" type="image/png" href="/images/favicon.png" />
    <link rel="icon" type="image/x-icon" href="/images/favicon.ico" />
    <link rel="shortcut icon" type="image/x-icon" href="/images/favicon.ico" />
    <link rel="canonical" href="<%= fileUrl %>" />
    <link
      rel="alternate"
      type="application/rss+xml"
      title="Nilesoft Shell Releases"
      href="https://nilesoft.org/feed/releases"
    />
    <link
      rel="alternate"
      type="application/rss+xml"
      title="Nilesoft Shell News"
      href="https://nilesoft.org/feed/news"
    />
    <link
      rel="alternate"
      type="application/rss+xml"
      title="Nilesoft Shell Blog"
      href="https://nilesoft.org/feed/blog"
    />
    <link
      rel="stylesheet"
      href="https://fonts.googleapis.com/css?family=Open+Sans"
    />
    <link rel="stylesheet" href="/assets/lib/bulma/bulma.min.css" />
    <link rel="stylesheet" href="/assets/css/main.css" />
    <link rel="stylesheet" href="/assets/css/glyphs.css" />

    <link rel="stylesheet" href="/assets/css/prism.css" />

    <!--
    <script
      async
      src="https://pagead2.googlesyndication.com/pagead/js/adsbygoogle.js?client=ca-pub-2559766466824022"
      crossorigin="anonymous"
    ></script>
      -->

    <script type="application/ld+json">
      {
        "@context": "https://schema.org",
        "@type": "SoftwareApplication",
        "name": "Nilesoft Shell",
        "softwareVersion": "1.8.1",
        "downloadUrl": "https://nilesoft.org/download/shell/1.8/setup.exe",
        "fileSize": "3.2MB",
        "operatingSystem": "Windows 7/8/10/11",
        "releaseNotes": "",
        "screenshot": "<%= rootUrl %>/images/screenshots/main.png",
        "applicationCategory": "DesktopEnhancementApplication"
      }
    </script>
  </head>
  <body id="mst-home">
    <main id="mst-main">
      <nav id="mst-nav" class="navbar mt-5" aria-label="main navigation">
        <div class="container">
          <div class="navbar-brand">
            <a class="navbar-item logo-shell" href="/">
              <svg viewBox="0 0 512 512" width="32" height="32">
                <path
                  fill="#000000"
                  d="M200 100L300 0l106 106-100 100zM100 200l100-100 212 212-100 100zM106 406l100-100 106 106-100 100z"
                />
              </svg>
            </a>
            <a
              role="button"
              class="navbar-burger"
              aria-label="menu"
              aria-expanded="false"
              data-target="navMenu"
            >
              <span aria-hidden="true"></span>
              <span aria-hidden="true"></span>
              <span aria-hidden="true"></span>
            </a>
          </div>
          <div id="navMenu" class="navbar-menu is-white has-text-centered">
            <div class="navbar-end">
              <a class="navbar-item is-lowercase" href="/">Home</a>
              <a class="navbar-item is-lowercase" href="/download">Download</a>
              <a class="navbar-item is-active is-lowercase" href="/docs"
                >Docs</a
              >
              <a class="navbar-item is-lowercase" href="/gallery/glyphs"
                >Glyphs</a
              >
              <a class="navbar-item is-lowercase" href="/donate">Donate</a>
              <a class="navbar-item is-lowercase" href="/blog">Blog</a>
              <a class="navbar-item is-lowercase" href="/contact">Contact</a>
            </div>
          </div>
        </div>
      </nav>
      <div class="is-hidden-mobile mb-6"></div>
      <div id="mst-content" class="container px-2">
        <h4 class="is-size-4 mb-5">Documents</h4>
        <section
          id="ad-responsive-horizontal-728-90"
          class="has-text-centered my-6"
        >
          <div
            style="
              width: 100%;
              height: 100px;
              border: 1px solid #0a0a0a;
              background-color: #808080;
            "
          >
            Advertisement
          </div>
          <!--
          <ins
            class="adsbygoogle"
            style="display: inline-block; width: 90%; height: 90px"
            data-ad-client="ca-pub-2559766466824022"
            data-ad-slot="6484658586"
            data-ad-format="auto"
            data-full-width-responsive="False"
          ></ins>
          <script>
            (adsbygoogle = window.adsbygoogle || []).push({});
          </script>
		-->
        </section>
        <div class="columns">
          <!-- #include file = "_aside.aspx" -->
          <div id="content" class="column">
            <div class="content">
    Hello World
              <hr />

              <pre>
     appRoot:        <%= appRoot.FullName %>
     projectRoot:    <%= projectRoot.FullName %>
     docsRoot:       <%= docsRoot.FullName %>
     requestUrl:     <%= requestUrl %>
     requestMap:     <%= requestMap %>
     requestedFile:  <%= requestedFile.FullName %>
     exists:         <%= requestedFile.Exists %>
     modified:       <%= fileModified %>
     accessed:       <%= Now() %>

     pageName:       <%= pageName %>
     pageSlug:       <%= pageSlug %>

     rootUrl:        <a href="<%= rootUrl %>"><%= rootUrl %></a>
     fileUrl:        <a href="<%= fileUrl %>"><%= fileUrl %></a>
     nilesoftUrl:    <a href="<%= nilesoftUrl %>" target="_blank"><%= nilesoftUrl %></a>
     gitUrl:         <a href="<%= gitUrl %>" target="_blank"><%= gitUrl %></a>

     SERVER_NAME:     <%= SERVER_NAME %>
     SERVER_PORT:     <%= SERVER_PORT %>
     HTTPS:           <%= HTTPS %>
 </pre>

              <div id="improve" class="my-5">
                <p class="">
                  This page is
                  <strong class="has-text-grey">open source</strong>. Noticed a
                  typo? Or something unclear?
                  <br />
                  <a
                    href="<%= gitUrl %>"
                    style="border-bottom: 1px solid currentColor"
                  >
                    Improve this page on GitHub
                  </a>
                </p>
              </div>
              <section
                id="ad-responsive-horizontal"
                class="has-text-centered my-6"
              >
                <div
                  style="
                    width: 100%;
                    height: 100px;
                    border: 1px solid #0a0a0a;
                    background-color: #808080;
                  "
                >
                  Advertisement
                </div>
                <!--
                <ins
                  class="adsbygoogle"
                  style="display: block"
                  data-ad-client="ca-pub-2559766466824022"
                  data-ad-slot="9611969124"
                  data-ad-format="auto"
                  data-full-width-responsive="True"
                ></ins>
                <script>
                  (adsbygoogle = window.adsbygoogle || []).push({});
                </script>
                -->
              </section>
            </div>
          </div>
        </div>
      </div>
    </main>
    <!-- #include file = "_footer.aspx" -->
  </body>
</html>
