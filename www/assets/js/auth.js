let isAuthenticated = false;
let isPassenger = false;

document.addEventListener("DOMContentLoaded", async () => {
    const authLinks = document.querySelectorAll("#auth-link");

    try {
        const response = await fetch('/api/auth', {
            method: 'GET',
            credentials: 'include'
        });

        if (response.ok) {
            const data = await response.json();

            if (data.status === "authorized") {
                isAuthenticated = true;
                if (data.pass) isPassenger = true;
                else {
                    const alrt = document.createElement('script');
                    alrt.src = "assets/js/alertdata.js";
                    document.body.appendChild(alrt);
                }
                authLinks.forEach(link => {
                    link.innerHTML = `
                        <li><a href="/public/account.html" class="hover:underline">${data.username}</a></li>
                        <li><a href="/index.html" onclick="logout()" class="hover:underline">Выйти</a></li>
                    `;
                });

                const script = document.createElement('script');
                script.src = "assets/js/logout.js";
                document.body.appendChild(script);
            }
        } else {
            console.warn("Пользователь не авторизован.");
        }
    } catch (error) {
        console.error("Ошибка при получении данных:", error);
    }
});

