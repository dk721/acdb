(function() {
    // Функция для создания и отображения всплывающего окна
    function showAlert() {
        // Создание элементов
        const alertBox = document.createElement('div');
        const alertMessage = document.createElement('span');
        const closeButton = document.createElement('span');
        const goToAccountLink = document.createElement('a');

        // Добавляем классы и стили
        alertBox.style.position = 'fixed';
        alertBox.style.bottom = '20px';
        alertBox.style.right = '20px';
        alertBox.style.backgroundColor = '#FFEB3B';
        alertBox.style.color = 'black';
        alertBox.style.padding = '10px';
        alertBox.style.borderRadius = '5px';
        alertBox.style.fontSize = '12px';
        alertBox.style.zIndex = '1000';
        alertBox.style.display = 'flex';
        alertBox.style.flexDirection = 'column';
        alertBox.style.alignItems = 'flex-start';
        alertBox.style.boxShadow = '0px 4px 6px rgba(0, 0, 0, 0.1)';
        alertBox.style.width = 'auto';

        alertMessage.textContent = 'Ваш аккаунт не заполнен полностью! Пожалуйста, заполните данные о пассажире.';
        alertMessage.style.marginRight = '12px';

        // Крестик для закрытия
        closeButton.textContent = '×';
        closeButton.style.position = 'absolute';
        closeButton.style.top = '0px';
        closeButton.style.right = '6px';
        closeButton.style.fontSize = '24px';
        closeButton.style.cursor = 'pointer';
        closeButton.style.color = 'black';
        
        goToAccountLink.textContent = 'Перейти в аккаунт';
        goToAccountLink.href = '/account.html';
        goToAccountLink.style.marginTop = '4px';
        goToAccountLink.style.textDecoration = 'underline';
        goToAccountLink.style.color = '#2196F3';

        alertBox.appendChild(closeButton);
        alertBox.appendChild(alertMessage);
        
        alertBox.appendChild(goToAccountLink);

        document.body.appendChild(alertBox);

        closeButton.addEventListener('click', function() {
            alertBox.remove();
        });

        setTimeout(function() {
            alertBox.remove();
        }, 5000);
    }

    const isPassenger = false;

    if (!isPassenger) {
        showAlert();
    }
})();
