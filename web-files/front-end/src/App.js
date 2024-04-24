import './App.css';
import Client from "./components/Client";
import SignUpLogin from "./components/SignUpLogin";
import {Route, Routes} from "react-router-dom";


function App() {
  return (
    <div className="App">
      <Routes>
        <Route path="/client" element={<Client/>}/>
        <Route path="/" element={<SignUpLogin/>}/>
      </Routes>

    </div>
  );
}

export default App;
