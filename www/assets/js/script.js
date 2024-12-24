document.addEventListener("DOMContentLoaded", () => {
    const themeToggle = document.getElementById("theme-toggle");
    const themeIconLight = document.getElementById("theme-icon-light");
    const themeIconDark = document.getElementById("theme-icon-dark");

    const mobileMenuButton = document.getElementById('mobile-menu-button');
    const mobileMenu = document.getElementById('mobile-menu');

    mobileMenuButton.addEventListener('click', () => {
        mobileMenu.classList.toggle('hidden');
    });

    const getCurrentTheme = () =>
        localStorage.getItem("theme") || (window.matchMedia("(prefers-color-scheme: dark)").matches ? "dark" : "light");

    const setTheme = (theme) => {
        document.documentElement.classList.toggle("dark", theme === "dark");

        if (theme === "dark") {
            themeIconDark.classList.remove("hidden");
            themeIconLight.classList.add("hidden");
        } else {
            themeIconLight.classList.remove("hidden");
            themeIconDark.classList.add("hidden");
        }

        localStorage.setItem("theme", theme);
    };

    const currentTheme = getCurrentTheme();
    setTheme(currentTheme);

    themeToggle.addEventListener("click", () => {
        const newTheme = getCurrentTheme() === "dark" ? "light" : "dark";
        setTheme(newTheme);
    });
});

(function () {
    const theme = localStorage.getItem('theme') || (window.matchMedia('(prefers-color-scheme: dark)').matches ? 'dark' : 'light');
    document.documentElement.classList.add(theme);
})();

const links = document.querySelectorAll('a[href^="#"]');
links.forEach(link => {
    link.addEventListener("click", function (e) {
        e.preventDefault();

        const targetId = this.getAttribute("href").substring(1);
        const targetElement = document.getElementById(targetId);

        window.scrollTo({
            top: targetElement.offsetTop - 100,
            behavior: "smooth"
        });
    });
});