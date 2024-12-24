function fetchFlights() {
    fetch('/api/reserves')
        .then(response => response.json())
        .then(reservedFlights => {
            const reservedFlightIds = new Set(reservedFlights.reserved_flights);

            fetch('/api/schedule')
                .then(response => response.json())
                .then(flights => {
                    const container = document.getElementById('flights-container');
                    container.innerHTML = '';

                    flights.forEach(flight => {
                        const departureDate = new Date(flight.departure_time);
                        const arrivalDate = new Date(flight.arrival_time);
                        const departureTime = departureDate.toLocaleString('ru-RU');
                        const arrivalTime = arrivalDate.toLocaleString('ru-RU');
                        const isReserved = reservedFlightIds.has(flight.id);

                        const flightHTML = `
                            <div class="shadow-lg rounded-lg p-6">
                                <h3 class="text-xl font-bold mb-2">
                                    ${flight.departure_airport} (${flight.departure_city}, ${flight.departure_country}) → 
                                    ${flight.arrival_airport} (${flight.arrival_city}, ${flight.arrival_country})
                                </h3>
                                <p>Дата вылета: ${departureTime}</p>
                                <p>Дата прибытия: ${arrivalTime}</p>
                                <p>Цена: ${flight.price} ₽</p>
                                <p class="text-sm text-gray-500">ID рейса: ${flight.id}</p>
                                <div class="mt-4">
                                    ${
                                        isAuthenticated
                                            ? `
                                                <button class="reserve-btn ok text-white py-2 px-4 rounded" data-flight-id="${flight.id}" ${isReserved ? 'style="display:none;"' : ''}>
                                                    Забронировать
                                                </button>
                                                <button class="cancel-btn bg-red-500 text-white py-2 px-4 rounded ${isReserved ? '' : 'hidden'}" data-flight-id="${flight.id}">
                                                    Отменить бронирование
                                                </button>
                                            `
                                            : `<button class="py-2 ok px-4 rounded cursor-not-allowed" disabled>
                                                Авторизуйтесь, чтобы бронировать
                                            </button>`
                                    }
                                </div>
                            </div>
                        `;
                        container.insertAdjacentHTML('beforeend', flightHTML);
                    });

                    // Добавляем обработчики событий, если пользователь авторизован
                    if (isAuthenticated) {
                        addEventListeners();
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


document.addEventListener('DOMContentLoaded', function() {
    if (typeof isAuthenticated === "undefined" || !isAuthenticated) {
        console.warn("Пользователь не авторизован. Отображение ограниченного функционала.");
    }
    fetchFlights();  // Загружаем рейсы
});
