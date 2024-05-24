import React, {useState} from 'react';
import '../css/sign.css';
import {useNavigate} from "react-router-dom";
import useAuth from "../auth/useAuth";

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
  const [userName, setUserName] = useState('');
  const [email, setEmail] = useState('');
  const [password, setPassword] = useState('');
  const navigate = useNavigate();
  const {signin} = useAuth();

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
          const userData = JSON.parse(data);
          if (userData.id) {
            localStorage.setItem('userEmail', email);
            localStorage.setItem('userId', userData.id);
            signin(userName, () => navigate('/client', {replace: true}));
          } else {
            alert();
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
  };

  // Добавим сохранение email при успешном входе
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
          const userData = JSON.parse(data);
          if (userData.id) {
            localStorage.setItem('userEmail', emailLogin);
            localStorage.setItem('userId', userData.id);
            signin(emailLogin, () => navigate('/client', {replace: true}));
          } else {
            alert(data);
          }
        })
        .catch(()=> {
          alert("Wrong email or password");
        });
    } else {
      if (!isValidEmail(emailLogin) || !isValidPassword(passwordLogin)) {
        alert("Wrong email or password");
      }
    }
  };
  return (
    <div className="body">
      <div className="main">
        <input type="checkbox" id="chk" aria-hidden="true"/>

        <div className="signup" id="signup_id">
          <form id="signup_form">
            <label htmlFor="chk" aria-hidden="true">Sign up</label>
            <input type="text" name="user_name_sign" placeholder="User name" required={true}
                   value={userName}
                   minLength={1}
                   onChange={(e) => setUserName(e.target.value)}/>
            <input type="email" name="email_sign" placeholder="Email" required={true}
                   value={email}
                   onChange={(e) => setEmail(e.target.value)}/>
            <input type="password" name="pswd_sign" placeholder="Password" required={true}
                   minLength={10}
                   value={password}
                   onChange={(e) => setPassword(e.target.value)}/>
            <button type="button" id="signup_button" onClick={handleSignUp} className='hover:bg-indigo-900'>Sign up</button>
          </form>
        </div>

        <div className="login" id="login_id">
          <form id="login_form">
            <label htmlFor="chk" aria-hidden="true">Login</label>
            <input type="email" name="email_log" placeholder="Email" required={true}
                   value={emailLogin}
                   onChange={(e) => setEmailLogin(e.target.value)}/>
            <input type="password" name="pswd_log" placeholder="Password" required={true}
                   value={passwordLogin}
                   onChange={(e) => setPasswordLogin(e.target.value)}/>
            <button type="button" id="login_button" onClick={handleLogin} className='hover:bg-indigo-900'>Login</button>
          </form>
        </div>
      </div>
    </div>
  );
};

export default SignUpLogin;
