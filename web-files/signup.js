$(document).ready(function () {
    window.$("#signup_button").on("click", function () {

        let userName = window.$("input[name='user_name']").val();
        let email = window.$("input[name='email']").val();
        let password = window.$("input[name='pswd']").val();

        if (userName && email && password) {
            let userData = {
                user_name: userName,
                email: email,
                pswd: password
            };

            let jsonStr = JSON.stringify(userData);

            window.$.ajax({
                url: "http://localhost:9001/signup",
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
                    alert("Error: " + error);
                }
            });
        } else {
            alert("Please fill in all fields: User name, Email, Password");
        }
    });
});