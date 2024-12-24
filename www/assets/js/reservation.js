document.addEventListener("DOMContentLoaded", () => {
    document.querySelectorAll(".reserve-btn").forEach(button => {
        button.addEventListener("click", async () => {
            const flightId = button.getAttribute("data-flight-id");
            try {
                const response = await fetch('/api/reserve', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    credentials: 'include',
                    body: JSON.stringify({ flight_id: parseInt(flightId) })
                });
                const data = await response.json();
                if (data.status === "success") {
                    alert("Рейс забронирован успешно!");
                } else {
                    alert(`Ошибка: ${data.message}`);
                }
            } catch (error) {
                console.error("Ошибка бронирования:", error);
            }
        });
    });

    document.querySelectorAll(".cancel-btn").forEach(button => {
        button.addEventListener("click", async () => {
            const flightId = button.getAttribute("data-flight-id");
            try {
                const response = await fetch('/api/cancel', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    credentials: 'include',
                    body: JSON.stringify({ flight_id: parseInt(flightId) })
                });
                const data = await response.json();
                if (data.status === "success") {
                    alert("Бронирование отменено успешно!");
                } else {
                    alert(`Ошибка: ${data.message}`);
                }
            } catch (error) {
                console.error("Ошибка отмены бронирования:", error);
            }
        });
    });
});
