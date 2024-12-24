document.addEventListener("DOMContentLoaded", async () => {
    const editButton = document.getElementById("edit-button");
    const editForm = document.getElementById("edit-form");
    const saveButton = document.getElementById("save-button");

    try {
        const response = await fetch('/api/userinfo', {
            method: 'GET',
            credentials: 'include',
        });

        if (!response.ok) {
            console.error("Failed to fetch user info:", response.statusText);
            return;
        }

        const data = await response.json();

        if (data.status === "success") {
            document.getElementById("username").textContent = data.username;
            document.getElementById("balance").textContent = `${data.balance}₽`;
            document.getElementById("first-name").textContent = data.passenger.first_name || "Не указано";
            document.getElementById("last-name").textContent = data.passenger.last_name || "Не указано";
            document.getElementById("passport").textContent = data.passenger.passport || "Не указано";
        } else {
            console.error("Error in response:", data.message);
        }
    } catch (err) {
        console.error("An error occurred while fetching user info:", err);
    }

    editButton.addEventListener("click", () => {
        editForm.classList.toggle("hidden");
    });

    saveButton.addEventListener("click", async () => {
        const newFirstName = document.getElementById("new-first-name").value.trim();
        const newLastName = document.getElementById("new-last-name").value.trim();
        const newPassport = document.getElementById("new-passport").value.trim();

        try {
            const response = await fetch("/api/update", {
                method: "POST",
                headers: {
                    "Content-Type": "application/json"
                },
                credentials: "include",
                body: JSON.stringify({
                    first_name: newFirstName,
                    last_name: newLastName,
                    passport_number: newPassport
                })
            });

            const result = await response.json();
            if (result.status === "success") {
                alert("Данные успешно обновлены!");
                location.reload();
            } else {
                alert(`Ошибка обновления: ${result.message}`);
            }
        } catch (err) {
            console.error("Ошибка отправки данных:", err);
        }
    });
});
