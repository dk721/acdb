let expandContainer = document.getElementById('expand-container');

let loginMessage = document.getElementById('login-message');
let registerMessage = document.getElementById('register-message');
let loginFormContainer = document.getElementById('login-form-container');
let registerFormContainer = document.getElementById('register-form-container');

function toggleExpandContainer() {
    expandContainer.classList.toggle('active');

    setTimeout(() => {
        loginMessage.classList.toggle('hidden');
        registerMessage.classList.toggle('hidden');
        loginFormContainer.classList.toggle('hidden');
        registerFormContainer.classList.toggle('hidden');
        expandContainer.classList.toggle('reverse');
        expandContainer.classList.toggle('active');
    }, 300);
}

function toggleForms() {
    expandContainer.classList.toggle('active');
    
    setTimeout(() => {
        loginMessage.classList.toggle('hidden');
        registerMessage.classList.toggle('hidden');
        loginFormContainer.classList.toggle('hidden');
        registerFormContainer.classList.toggle('hidden');
        
        expandContainer.classList.toggle('reverse');
        expandContainer.classList.toggle('active');
    }, 300);
}

document.getElementById('register-form').addEventListener('submit', function(e) {
    e.preventDefault();
    
    const formData = {
        username: document.getElementById('register-username').value,
        email: document.getElementById('register-email').value,
        password: document.getElementById('register-password').value,
    };

    fetch('/api/register', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(formData)
    })
    .then(response => response.json())
    .then(data => {
        if (data["status"] == "registered")
            window.location.href = '/public/account.html';
    })
    .catch(error => {
        console.error('Ошибка:', error);
        alert('Произошла ошибка');
    });
});

document.getElementById('login-form').addEventListener('submit', function(e) {
    e.preventDefault();
    
    const formData = {
        username: document.getElementById('username').value,
        password: document.getElementById('password').value,
    };

    fetch('/api/login', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(formData)
    })
    .then(response => response.json())
    .then(data => {
        if (data["status"] == "logged in") window.location.href = '/public/account.html';
    })
    .catch(error => {
        console.error('Ошибка:', error);
        alert('Произошла ошибка');
    });
});