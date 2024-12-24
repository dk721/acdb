function logout() {
    try {
        const response = fetch('/api/logout', {
            method: 'POST',
            credentials: 'include'
        });

        if (response.ok) {
            const data = response.json();
            if (data.status === "logged out") {
                document.cookie = "session_id=; Max-Age=0; path=/;";
            } else {
                console.error("Ошибка выхода:", data);
            }
        } else {
            console.error("Серверная ошибка при выходе");
        }
    } catch (error) {
        console.error("Ошибка сети при выходе:", error);
    }
}