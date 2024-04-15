var bcrypt = dcodeIO.bcrypt;

function loginUser(email, password) {
    var salt = bcrypt.genSaltSync(10);
    var passwordToSave = bcrypt.hashSync(password, salt)
    return {
        email,
        pswd: passwordToSave
    };
}

$(document).ready(function () {
    window.$("#login_button").on("click", function () {

        let email = window.$("input[name='email_log']").val();
        let password = window.$("input[name='pswd_log']").val();

        if (email && password) {
            let userData = loginUser(email, password);

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
            window.location.replace("client.html");
            alert("Success");
        }
        else
        {
            alert("Please fill in all fields:Email, Password");
        }
    });
});