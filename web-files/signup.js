var bcrypt = dcodeIO.bcrypt;
var salt = bcrypt.genSaltSync(13);

function createUser(userName, email, password) {
    var passwordToSave = bcrypt.hashSync(password, salt);
    return {
        user_name: userName,
        email,
        pswd: passwordToSave
    };
}
function isValidEmail(email) {
    const emailPattern = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return emailPattern.test(email.trim());
}

function isValidPassword(password) {
    return password.length >= 10;
}
function isValidNickname(nickname) {
    return (nickname.length <= 20) && (nickname.length > 0) && !/^\d+$/.test(nickname);
}

$(document).ready(function () {
    window.$("#signup_button").on("click", function () {

        let userName = window.$("input[name='user_name_sign']").val();
        let email = window.$("input[name='email_sign']").val();
        let password = window.$("input[name='pswd_sign']").val();

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
                alert("your name should be short and contain more than just numbers");
            }
            if (!isValidEmail(email)) {
                alert("Invalid email");
            }
            if (!isValidPassword(password)) {
                alert("Your password must be at least 10 characters");
            }
        }
    });
});