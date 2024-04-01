$(document).ready(function () {
    $("#signup_button").on("click", function () {

        var userData = {
            txt: $("input[name='txt']").val(),
            email: $("input[name='email']").val(),
            pswd: $("input[name='pswd']").val()
        };

        let jsonStr = JSON.stringify(userData);

        $.ajax({
            url: "http://localhost:9001/signup",
            method: "post",
            contentType: "application/json",
            data: jsonStr,
            success: function (data) {
                alert("Registration successful!");
                console.log(data);
            },
            error: function (xhr, status, error) {
                alert("Error: " + error);
                // Обработка ошибки, если необходимо
            }
        });
    });
});
