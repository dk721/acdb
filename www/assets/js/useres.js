function fetchReservedFlights() {
    // Получаем список забронированных рейсов
    fetch('/api/reserves')
        .then(response => response.json())
        .then(reservedFlights => {
            // Преобразуем ответ (массив чисел) в Set для быстрого поиска
            const reservedFlightIds = new Set(reservedFlights.reserved_flights);

            // Получаем список всех рейсов
            fetch('/api/schedule')
                .then(response => response.json())
                .then(flights => {
                    const container = document.getElementById('flights-container');
                    container.innerHTML = '';  // Очистка контейнера перед добавлением новых данных

                    // Флаг, чтобы проверить, есть ли забронированные рейсы
                    let hasReservedFlights = false;

                    flights.forEach(flight => {
                        if (reservedFlightIds.has(flight.id)) {
                            hasReservedFlights = true;  // Есть забронированный рейс

                            // Преобразуем дату и время в читаемый формат
                            const departureDate = new Date(flight.departure_time);
                            const arrivalDate = new Date(flight.arrival_time);

                            const departureTime = departureDate.toLocaleString('ru-RU'); // локализованный формат времени
                            const arrivalTime = arrivalDate.toLocaleString('ru-RU');

                            const flightHTML = `
                                <div class="shadow-lg rounded-lg p-6">
                                    <h3 class="text-xl font-bold mb-2">${flight.departure_airport} (${flight.departure_city}, ${flight.departure_country}) → ${flight.arrival_airport} (${flight.arrival_city}, ${flight.arrival_country})</h3>
                                    <p>Дата вылета: ${departureTime}</p>
                                    <p>Дата прибытия: ${arrivalTime}</p>
                                    <p>Цена: ${flight.price} ₽</p>
                                    <p class="text-sm text-gray-500">ID рейса: ${flight.id}</p>
                                    <button class="cancel-btn mt-4 bg-red-500 text-white py-2 px-4 rounded hover:bg-red-600" data-flight-id="${flight.id}">
                                        Отменить бронирование
                                    </button>
                                </div>
                            `;
                            container.insertAdjacentHTML('beforeend', flightHTML);
                        }
                    });

                    // Если не было найдено забронированных рейсов, показываем сообщение
                    if (!hasReservedFlights) {
                        container.innerHTML = `
                            <div class="bg-white p-6 rounded-lg shadow-lg">
                                <p class="text-center text-gray-500">Тут пусто. У вас нет забронированных рейсов.</p>
                            </div>
                        `;
                    } else {
                        // Добавляем обработчики событий для кнопок "Отменить бронирование"
                        addCancelEventListeners();
                    }
                })
                .catch(error => {
                    console.error('Ошибка при получении рейсов:', error);
                });
        })
        .catch(error => {
            console.error('Ошибка при получении забронированных рейсов:', error);
        });
}

function addCancelEventListeners() {
    // Обработчик для кнопок "Отменить бронирование"
    document.querySelectorAll('.cancel-btn').forEach(button => {
        button.addEventListener('click', async () => {
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
                    // Убираем рейс из отображения
                    button.closest('div').remove();
                    // Если больше нет рейсов, добавляем сообщение
                    const container = document.getElementById('flights-container');
                    if (container.children.length === 0) {
                        container.innerHTML = `
                            <div class="bg-white p-6 rounded-lg shadow-lg">
                                <p class="text-center text-gray-500">Тут пусто. У вас нет забронированных рейсов.</p>
                            </div>
                        `;
                    }
                } else {
                    alert(`Ошибка: ${data.message}`);
                }
            } catch (error) {
                console.error("Ошибка отмены бронирования:", error);
            }
        });
    });
}

// Загружаем забронированные рейсы при загрузке страницы
document.addEventListener('DOMContentLoaded', function() {
    fetchReservedFlights();
});
