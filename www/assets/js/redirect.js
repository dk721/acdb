document.addEventListener("DOMContentLoaded", async () => {
    try {
        const response = await fetch('/api/auth', {
            method: 'GET',
            credentials: 'include'
        });

        if (response.ok) {
            const data = await response.json();

            if (data.status === "authorized") {
                window.location.href = '/public/account.html';
            }
        } else {
            console.warn("Не удалось проверить авторизацию:", response.statusText);
        }
    } catch (error) {
        console.error("Ошибка при проверке авторизации:", error);
    }
});
