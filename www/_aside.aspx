<%@ Import Namespace="System.IO" %>

<aside id="categories" class="column is-one-quarter px-2">
  <div class="menu">
    <div class="container">
      <ul class="menu-list">
        <li>
          <a href="/docs">
            <span class="ml-5">Introduction</span>
          </a>
        </li>
        <% Response.Write(PrintNav(docsRoot).toString()) %>
      </ul>
    </div>
  </div>
  <section class="container p-3 has-text-centered my-6">
    <div
      style="
        width: 100%;
        height: 500px;
        border: 1px solid #0a0a0a;
        background-color: #808080;
      "
    >
      Advertisement
    </div>
    <!--  <ins
        class="adsbygoogle"
        style="display: block"
        data-ad-client="ca-pub-2559766466824022"
        data-ad-slot="2410467654"
        data-ad-format="auto"
        data-full-width-responsive="True"
        ></ins>
        <script>
        (adsbygoogle = window.adsbygoogle || []).push({});
        </script>-->
  </section>
</aside>
