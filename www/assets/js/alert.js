// Функция для создания кастомного alert
function createCustomAlert() {
    // Создаём элементы для модального окна
    const alertContainer = document.createElement("div");
    alertContainer.id = "custom-alert";
    alertContainer.classList.add("fixed", "inset-0", "bg-gray-900", "bg-opacity-50", "hidden", "flex", "justify-center", "items-center", "z-20");

    const alertBox = document.createElement("div");
    alertBox.classList.add("rounded-lg", "p-6", "w-96", "text-center", "border", "shadow-lg");

    const alertTitle = document.createElement("h3");
    alertTitle.id = "alert-title";
    alertTitle.classList.add("text-2xl", "font-semibold", "mb-4");
    alertTitle.textContent = "Сообщение";

    const alertMessage = document.createElement("p");
    alertMessage.id = "alert-message";
    alertMessage.classList.add("mb-4");
    alertMessage.textContent = "Текст сообщения";

    const closeButton = document.createElement("button");
    closeButton.id = "alert-close";
    closeButton.classList.add("py-2", "px-4", "rounded", "text-white", "hover:opacity-75");
    closeButton.textContent = "Закрыть";

    // Добавляем элементы в alertBox
    alertBox.appendChild(alertTitle);
    alertBox.appendChild(alertMessage);
    alertBox.appendChild(closeButton);

    // Добавляем alertBox в alertContainer
    alertContainer.appendChild(alertBox);

    // Добавляем alertContainer в body
    document.body.appendChild(alertContainer);

    // Закрытие модального окна
    closeButton.addEventListener("click", function() {
        alertContainer.classList.remove("show");
    });

    // Закрытие окна при клике вне его
    alertContainer.addEventListener("click", function(event) {
        if (event.target === alertContainer) {
            alertContainer.classList.remove("show");
        }
    });

    // Функция для отображения кастомного alert
    function showAlert(message, close = 'Закрыть', type = 'info') {
        alertMessage.textContent = message;

        // Очистка всех классов для alertBox, alertTitle, alertMessage и closeButton
        const allElements = [alertBox, alertTitle, alertMessage, closeButton];
        allElements.forEach(el => {
            el.classList.remove(
                "bg-green-100", "bg-red-100", "bg-blue-100", "bg-gray-100",
                "border-green-500", "border-red-500", "border-blue-500", "border-gray-500",
                "text-green-700", "text-red-700", "text-blue-700", "text-gray-700",
                "bg-green-500", "bg-red-500", "bg-blue-500", "bg-gray-500",
                "hover:bg-green-600", "hover:bg-red-600", "hover:bg-blue-600", "hover:bg-gray-600"
            );
        });

        // Настройка классов для каждого типа сообщения
        if (type === 'success') {
            alertTitle.textContent = "Успех!";
            alertBox.classList.add("bg-green-100", "border-green-500");
            alertTitle.classList.add("text-green-700");
            alertMessage.classList.add("text-green-700");
            closeButton.classList.add("bg-green-500", "hover:bg-green-600");
        } else if (type === 'error') {
            alertTitle.textContent = "Ошибка!";
            alertBox.classList.add("bg-red-100", "border-red-500");
            alertTitle.classList.add("text-red-700");
            alertMessage.classList.add("text-red-700");
            closeButton.classList.add("bg-red-500", "hover:bg-red-600");
        } else if (type === 'info') {
            alertTitle.textContent = "Информация";
            alertBox.classList.add("bg-blue-100", "border-blue-500");
            alertTitle.classList.add("text-blue-700");
            alertMessage.classList.add("text-blue-700");
            closeButton.classList.add("bg-blue-500", "hover:bg-blue-600");
        } else {
            alertTitle.textContent = "Сообщение";
            alertBox.classList.add("bg-gray-100", "border-gray-500");
            alertTitle.classList.add("text-gray-700");
            alertMessage.classList.add("text-gray-700");
            closeButton.classList.add("bg-gray-500", "hover:bg-gray-600");
        }

        // Показываем модальное окно
        closeButton.textContent = close;
        alertContainer.classList.add("show");
    }

    // Возвращаем функцию показа alert
    return showAlert;
}

// Добавляем стили для кастомного alert
function addCustomAlertStyles() {
    const style = document.createElement("style");
    style.textContent = `
        #custom-alert {
            display: flex;
            position: fixed;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: rgba(0, 0, 0, 0.5);
            justify-content: center;
            align-items: center;
            z-index: 9999;
            visibility: hidden;
            opacity: 0;
            transition: visibility 0s, opacity 0.3s ease;
        }
        #custom-alert.show {
            visibility: visible;
            opacity: 1;
        }

        #custom-alert .rounded-lg {
            border-radius: 0.5rem;
        }
        #custom-alert .p-6 {
            padding: 1.5rem;
        }
        #custom-alert .w-96 {
            width: 24rem;
        }

        #custom-alert .border {
            border-width: 2px;
            border-radius: 0.5rem;
        }
        #custom-alert .text-center {
            text-align: center;
        }

        #custom-alert .bg-green-100 {
            background-color: #d1fad1;
        }
        #custom-alert .border-green-500 {
            border-color: #48bb78;
        }
        #custom-alert .text-green-700 {
            color: #2f855a;
        }
        #custom-alert .bg-green-500 {
            background-color: #48bb78;
        }
        #custom-alert .hover\:bg-green-600:hover {
            background-color: #38a169;
        }

        #custom-alert .bg-red-100 {
            background-color: #fee2e2;
        }
        #custom-alert .border-red-500 {
            border-color: #f56565;
        }
        #custom-alert .text-red-700 {
            color: #c53030;
        }
        #custom-alert .bg-red-500 {
            background-color: #f56565;
        }
        #custom-alert .hover\:bg-red-600:hover {
            background-color: #e53e3e;
        }

        #custom-alert .bg-blue-100 {
            background-color: #ebf8ff;
        }
        #custom-alert .border-blue-500 {
            border-color: #3b82f6;
        }
        #custom-alert .text-blue-700 {
            color: #1e40af;
        }
        #custom-alert .bg-blue-500 {
            background-color: #3b82f6;
        }
        #custom-alert .hover\:bg-blue-600:hover {
            background-color: #2563eb;
        }

        #custom-alert .text-2xl {
            font-size: 1.5rem;
        }

        #custom-alert .font-semibold {
            font-weight: 600;
        }

        #custom-alert .text-white {
            color: #fff;
        }

        #custom-alert .py-2 {
            padding-top: 0.5rem;
            padding-bottom: 0.5rem;
        }
        #custom-alert .px-4 {
            padding-left: 1rem;
            padding-right: 1rem;
        }

        #custom-alert .rounded {
            border-radius: 0.375rem;
        }
    `;
    document.head.appendChild(style);
}

addCustomAlertStyles();
const showAlert = createCustomAlert();
