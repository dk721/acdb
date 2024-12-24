function addEventListeners() {
    // Обработчик для кнопок "Забронировать"
    document.querySelectorAll('.reserve-btn').forEach(button => {
        button.addEventListener('click', async () => {
            if (isPassenger == false) {
                alert("Сначала обновите данные в личном кабинете");
                return;
            }
            const flightId = button.getAttribute('data-flight-id');
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
                    // Обновляем состояние кнопок
                    button.style.display = 'none';  // Скрываем кнопку "Забронировать"
                    const cancelButton = button.nextElementSibling;
                    cancelButton.style.display = 'inline-block';  // Показываем кнопку "Отменить бронирование"
                } else {
                    alert(`Ошибка: ${data.message}`);
                }
            } catch (error) {
                console.error("Ошибка бронирования:", error);
            }
        });
    });

    // Обработчик для кнопок "Отменить бронирование"
    document.querySelectorAll('.cancel-btn').forEach(button => {
        button.addEventListener('click', async () => {
            if (isPassenger == false) {
                alert("Сначала обновите данные в личном кабинете");
                return;
            }
            const flightId = button.getAttribute('data-flight-id');
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
                    // Обновляем состояние кнопок
                    button.style.display = 'none';  // Скрываем кнопку "Отменить бронирование"
                    const reserveButton = button.previousElementSibling;
                    reserveButton.style.display = 'inline-block';  // Показываем кнопку "Забронировать"
                } else {
                    alert(`Ошибка: ${data.message}`);
                }
            } catch (error) {
                console.error("Ошибка отмены бронирования:", error);
            }
        });
    });
}