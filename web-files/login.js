function isValidEmail(email) {
    const emailPattern = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return emailPattern.test(email.trim());
}

function isValidPassword(password) {
    return password.length >= 10;
}

$(document).ready(function () {
    window.$("#login_button").on("click", function () {

        let email = window.$("input[name='email_log']").val();
        let password = window.$("input[name='pswd_log']").val();

        if (isValidEmail(email) && isValidPassword(password)) {
            let userData = {
                email: email,
                pswd: password
            };

            let jsonStr = JSON.stringify(userData);

            window.$.ajax({
                url: "http://localhost:9000/login",
                method: "post",
                contentType: "application/json",
                data: jsonStr,
                success: function (data, textStatus, xhr) {
                    if (xhr.status === 200) {
                        alert(data);
                    }
                    else if (xhr.status === 409) // User with this email already exists
                    {
                        alert("Error: " + data);
                    }
                    else {
                        alert("Error: " + data);
                    }
                },
                error: function (xhr, status, error) {
                    alert("Error: " + error);
                }
            });
            window.location.replace("client.html");
        }
        else
        {
            if (!isValidEmail(email)) {
                alert("Invalid email");
            }
            if (!isValidPassword(password)) {
                alert("Your password must be at least 10 characters");
            }
        }
    });
});