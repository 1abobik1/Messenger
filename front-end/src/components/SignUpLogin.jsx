import React, { useState } from 'react';
import '../css/sign.css'
import { useNavigate } from "react-router-dom";

var bcrypt = require('bcryptjs');
var salt = bcrypt.genSaltSync(13);

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

function createUser(userName, email, password) {
    var passwordToSave = bcrypt.hashSync(password, salt);
    return {
        user_name: userName,
        email,
        pswd: passwordToSave
    };
}


const SignUpLogin = () => {

    // Ïðè ðåãèñòðàöèè
    const [userName, setUserName] = useState('');
    const [email, setEmail] = useState('');
    const [password, setPassword] = useState('');
    let navigate = useNavigate();


    const handleSignUp = () => {
        if (isValidNickname(userName) && isValidEmail(email) && isValidPassword(password)) {
            let userData = createUser(userName, email, password);
            let jsonStr = JSON.stringify(userData);

            fetch('http://localhost:9000/signup', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: jsonStr
            })
                .then(response => {
                    if (response.ok) {
                        return response.text();
                    } else {
                        throw new Error('Request failed');
                    }
                })
                .then(data => {
                    if (data === 'Signup successful!') {
                        navigate('/client', { replace: true });
                    } else {
                        alert(data);
                    }
                })
                .catch(error => {
                    alert(error.message);
                });

        } else {
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
    }


    // Ïðè ëîãèíå
    const [emailLogin, setEmailLogin] = useState('');
    const [passwordLogin, setPasswordLogin] = useState('');

    const handleLogin = () => {
        if (isValidEmail(emailLogin) && isValidPassword(passwordLogin)) {
            let userData = {
                email: emailLogin,
                pswd: passwordLogin
            };

            let jsonStr = JSON.stringify(userData);
            fetch('http://localhost:9000/login', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: jsonStr
            })
                .then(response => {
                    if (response.ok) {
                        return response.text();
                    } else {
                        throw new Error('Request failed');
                    }
                })
                .then(data => {
                    if (data === 'LogIn successful!') {
                        navigate('/client', { replace: true });
                    } else {
                        alert(data);
                    }
                })
                .catch(error => {
                    alert(error.message);
                });
        } else {
            if (!isValidEmail(emailLogin)) {
                alert("Wrong email");
            }
            if (!isValidPassword(passwordLogin)) {
                alert("Wrong password");
            }
        }
    };

    return (
        <div className="body">
            <div className="main">
                <input type="checkbox" id="chk" aria-hidden="true" />

                <div className="signup" id="signup_id">
                    <form id="signup_form">
                        <label htmlFor="chk" aria-hidden="true">Sign up</label>
                        <input type="text" name="user_name_sign" placeholder="User name" required=""
                            value={userName}
                            onChange={(e) => setUserName(e.target.value)} />
                        <input type="email" name="email_sign" placeholder="Email" required=""
                            value={email}
                            onChange={(e) => setEmail(e.target.value)} />
                        <input type="password" name="pswd_sign" placeholder="Password" required=""
                            value={password}
                            onChange={(e) => setPassword(e.target.value)} />
                        <button type="button" id="signup_button" onClick={handleSignUp}>Sign up</button>
                    </form>
                </div>

                <div className="login" id="login_id">
                    <form id="login_form">
                        <label htmlFor="chk" aria-hidden="true">Login</label>
                        <input type="email" name="email_log" placeholder="Email" required=""
                            value={emailLogin}
                            onChange={(e) => setEmailLogin(e.target.value)} />
                        <input type="password" name="pswd_log" placeholder="Password" required=""
                            value={passwordLogin}
                            onChange={(e) => setPasswordLogin(e.target.value)} />
                        <button type="button" id="login_button" onClick={handleLogin}>Login</button>
                    </form>
                </div>
            </div>
        </div>


    );
};

export default SignUpLogin;