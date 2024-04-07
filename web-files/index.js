$(document).ready(function () {
    window.$("#signup_button").on("click", function () {
        let userData = {
            user_name: window.$("input[name='user_name']").val(),
            email: window.$("input[name='email']").val(),
            pswd: window.$("input[name='pswd']").val()
        };

        let jsonStr = JSON.stringify(userData);

        window.$.ajax({
            url: "http://localhost:9001/signup",
            method: "post",
            contentType: "application/json",
            data: jsonStr,
            success: function (data) {
                alert("Registration successful!");
            }
            //error: function (xhr, status, error) {
            //    alert("Error: " + error);
            //    // Обработка ошибки, если необходимо
            //}
        });
    });
});