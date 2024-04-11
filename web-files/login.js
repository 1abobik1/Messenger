$(document).ready(function () {
    window.$("#login_button").on("click", function () {

        let email = window.$("input[name='email']").val();
        let password = window.$("input[name='pswd']").val();

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
                success: function (data, textStatus, xhr) {
                    if (xhr.status === 200) {
                        alert(data);
                    }
                    else if (xhr.status === 409)
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
        }
        else
        {
            alert("Please fill in all fields:Email, Password");
        }
    });
});