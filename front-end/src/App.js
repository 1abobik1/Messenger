import './App.css';
import Client from "./components/Client";
import SignUpLogin from "./components/SignUpLogin";
import {Route, Routes} from "react-router-dom";
import RequireAuth from "./auth/RequireAuth";
import {AuthProvider} from "./auth/AuthProvider";
import NotFound from "./components/NotFound";
import ClientID from "./components/ClientId";
import ClientId from "./components/ClientId";


function App() {
  return (
    <div className="App">
      <AuthProvider>
        <Routes>
          <Route path="/client" element={
            <RequireAuth>
              <Client/>
            </RequireAuth>
          }/>
          <Route path="/client/:id" element={
            <RequireAuth>
              <ClientId/>
            </RequireAuth>
          }/>
          <Route path="/" element={<SignUpLogin/>}/>
          <Route path="*" element={<NotFound/>}/>
        </Routes>
      </AuthProvider>
    </div>
  );
}

export default App;
