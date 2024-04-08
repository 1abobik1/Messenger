$(document).ready(function () {
    window.$("#login_button").on("click", function () {

        let email = window.$("input[name='login_email']").val();
        let password = window.$("input[name='login_pswd']").val();

        if (email && password) {
            let userData = {
                email: email,
                pswd: password
            };

            let jsonStr = JSON.stringify(userData);

            window.$.ajax({
                url: "http://localhost:9001/login",
                method: "post",
                contentType: "application/json",
                data: jsonStr,
                success: function (data) {
                    alert("LogIn successful!");
                },
                error: function (xhr, status, error) {
                    alert("Error: " + error);
                }
            });
        } else {
            alert("Please fill in all fields:Email, Password");
        }
    });
});