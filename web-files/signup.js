var bcrypt = dcodeIO.bcrypt;

function createUser(userName, email, password) {
    var salt = bcrypt.genSaltSync(10);
    var passwordToSave = bcrypt.hashSync(password, salt)
    return {
        user_name: userName,
        email,
        pswd: passwordToSave
    };
}

function isValidEmail(email) {
    const emailPattern = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return emailPattern.test(email);
}
function isValidPassword(password) {
    return password.length >= 10;
}
function isValidNickname(nickname) {
    return (nickname.length <= 20) && (nickname.length > 0) && !/^\d+$/.test(nickname);
}

$(document).ready(function () {
    window.$("#signup_button").on("click", function () {

        let userName = window.$("input[name='user_name']").val();
        let email = window.$("input[name='email']").val();
        let password = window.$("input[name='pswd']").val();

        if (isValidNickname(userName) && isValidEmail(email) && isValidPassword(password)) {
            let userData = createUser(userName, email, password);

            let jsonStr = JSON.stringify(userData);

            window.$.ajax({
                url: "http://localhost:9000/signup",
                method: "post",
                contentType: "application/json",
                data: jsonStr,
                success: function (data, textStatus, xhr) {
                    if (xhr.status === 200)
                    {
                        alert(data);
                    }
                    else if (xhr.status === 409) // User with this email already exists
                    {
                        alert("Error: " + data);
                    }
                    else
                    {
                        alert("Error: " + data);
                    }
                },
                error: function (xhr, status, error) {
                    alert("The site is temporarily down");
                }
            });
            window.location.replace("client.html");
        }
        else
        {
            if (!isValidNickname(userName)) {
                alert("Ваш ник должен быть не более 20 символов и не должен состоять только из цифр")
            }
            if (!isValidEmail(email)) {
                alert("Введен некорректный email")
            }
            if (!isValidPassword(password)) {
                alert("Ваш пароль должен быть не менее 10 символов")
            }
        }
    });
});