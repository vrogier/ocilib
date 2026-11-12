/* OCILIB API doc — theme toggle. No dependencies.
   Shares the "ocilib-tut-theme" localStorage key with the tutorials
   and the website so the choice follows the reader across all three. */
(function () {
    "use strict";

    var THEME_KEY = "ocilib-tut-theme";

    var themeToggle = document.getElementById("theme-toggle");

    function applyTheme(theme) {
        document.documentElement.setAttribute("data-theme", theme);
        if (themeToggle) themeToggle.textContent = theme === "dark" ? "☀" : "◐";
    }

    if (themeToggle) {
        themeToggle.addEventListener("click", function () {
            var theme = document.documentElement.getAttribute("data-theme") === "dark" ? "light" : "dark";
            applyTheme(theme);
            try { localStorage.setItem(THEME_KEY, theme); } catch (e) { }
        });
        // sync button icon with the theme applied by the head bootstrap
        applyTheme(document.documentElement.getAttribute("data-theme") || "light");
    }
})();
