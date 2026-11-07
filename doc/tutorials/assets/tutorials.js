/* OCILIB tutorials — tabs, theme, copy buttons, mobile nav. No dependencies. */
(function () {
    "use strict";

    var LANG_KEY = "ocilib-tut-lang";
    var THEME_KEY = "ocilib-tut-theme";
    var DEFAULT_LANG = "c";

    /* ------------------------------------------------------------ language tabs */

    function setActivePane(sample, lang) {
        var panes = sample.querySelectorAll(".code-pane");
        var tabs = sample.querySelectorAll(".code-tab");
        var found = false;
        panes.forEach(function (pane) {
            var match = pane.getAttribute("data-lang") === lang;
            pane.hidden = !match;
            pane.classList.toggle("active", match);
            if (match) found = true;
        });
        if (!found) {          // keep the first pane if this sample has no such tab
            panes.forEach(function (pane, i) {
                pane.hidden = i !== 0;
                pane.classList.toggle("active", i === 0);
            });
            lang = panes.length ? panes[0].getAttribute("data-lang") : lang;
        }
        tabs.forEach(function (tab) {
            tab.classList.toggle("active", tab.getAttribute("data-lang") === lang);
        });
    }

    function applyLanguage(lang) {
        document.querySelectorAll(".code-sample").forEach(function (sample) {
            setActivePane(sample, lang);
        });
    }

    document.querySelectorAll(".code-tab").forEach(function (tab) {
        tab.addEventListener("click", function () {
            var lang = tab.getAttribute("data-lang");
            try { localStorage.setItem(LANG_KEY, lang); } catch (e) { /* file:// is fine */ }
            applyLanguage(lang);
        });
    });

    var savedLang = DEFAULT_LANG;
    try { savedLang = localStorage.getItem(LANG_KEY) || DEFAULT_LANG; } catch (e) { }
    applyLanguage(savedLang);

    /* -------------------------------------------------------------- copy button */

    document.querySelectorAll(".code-copy").forEach(function (btn) {
        btn.addEventListener("click", function () {
            var sample = btn.closest(".code-sample");
            var pane = sample && sample.querySelector(".code-pane:not([hidden])");
            if (!pane) return;
            var text = pane.innerText;
            var done = function () {
                btn.textContent = "Copied";
                btn.classList.add("copied");
                setTimeout(function () {
                    btn.textContent = "Copy";
                    btn.classList.remove("copied");
                }, 1400);
            };
            if (navigator.clipboard && navigator.clipboard.writeText) {
                navigator.clipboard.writeText(text).then(done, function () { });
            }
        });
    });

    /* ----------------------------------------------------------------- theme */

    var themeToggle = document.getElementById("theme-toggle");

    function applyTheme(theme) {
        document.documentElement.setAttribute("data-theme", theme);
        if (themeToggle) themeToggle.textContent = theme === "dark" ? "☀" : "◐";
    }

    var theme = "light";
    try {
        theme = localStorage.getItem(THEME_KEY) ||
            (window.matchMedia && window.matchMedia("(prefers-color-scheme: dark)").matches
                ? "dark" : "light");
    } catch (e) { }
    applyTheme(theme);

    if (themeToggle) {
        themeToggle.addEventListener("click", function () {
            theme = document.documentElement.getAttribute("data-theme") === "dark" ? "light" : "dark";
            try { localStorage.setItem(THEME_KEY, theme); } catch (e) { }
            applyTheme(theme);
        });
    }

    /* ------------------------------------------------- collapsible sidebar parts */

    var NAV_KEY = "ocilib-tut-nav";

    function persistNavState() {
        var open = [];
        document.querySelectorAll(".nav-details[open]").forEach(function (d) {
            open.push(d.getAttribute("data-part"));
        });
        try { localStorage.setItem(NAV_KEY, JSON.stringify(open)); } catch (e) { }
    }

    try {
        var savedNav = JSON.parse(localStorage.getItem(NAV_KEY) || "null");
        if (savedNav) {
            document.querySelectorAll(".nav-details").forEach(function (d) {
                var hasActive = !!d.querySelector("li.active");
                d.open = hasActive || savedNav.indexOf(d.getAttribute("data-part")) !== -1;
            });
        }
    } catch (e) { }

    document.querySelectorAll(".nav-details").forEach(function (d) {
        d.addEventListener("toggle", persistNavState);
    });

    /* -------------------------------------------------------------- mobile nav */

    var navToggle = document.getElementById("nav-toggle");
    var sidebar = document.getElementById("sidebar");
    var scrim = document.getElementById("sidebar-scrim");

    function closeNav() {
        if (sidebar) sidebar.classList.remove("open");
        if (scrim) scrim.classList.remove("show");
    }

    if (navToggle && sidebar) {
        navToggle.addEventListener("click", function () {
            sidebar.classList.toggle("open");
            if (scrim) scrim.classList.toggle("show", sidebar.classList.contains("open"));
        });
    }
    if (scrim) scrim.addEventListener("click", closeNav);
    if (sidebar) {
        sidebar.addEventListener("click", function (ev) {
            if (ev.target.tagName === "A") closeNav();
        });
    }
})();
