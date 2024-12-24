document.addEventListener("DOMContentLoaded", () => {
    const form = document.getElementById('payment-form');
    
    form.addEventListener('submit', async (event) => {
        event.preventDefault();

        const amount = document.getElementById('amount').value;

        if (!amount || amount <= 0) {
            alert("Пожалуйста, введите корректную сумму.");
            return;
        }

        try {
            const response = await fetch('/api/payment', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                credentials: 'include',
                body: JSON.stringify({ amount: amount })
            });

            const data = await response.json();

            if (response.ok && data.status === "success") {
                alert('Пополнение успешно!');
                window.location.href = '/public/account.html';
            } else {
                alert('Ошибка пополнения: ' + (data.message || 'Попробуйте еще раз.'));
            }
        } catch (error) {
            console.error("Ошибка при отправке запроса:", error);
            alert("Произошла ошибка. Попробуйте снова.");
        }
    });
});
