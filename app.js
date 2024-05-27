import { initializeApp } from "https://www.gstatic.com/firebasejs/10.12.1/firebase-app.js";
import { getDatabase, ref, onValue } from "https://www.gstatic.com/firebasejs/10.12.1/firebase-database.js";

const firebaseConfig = {
  apiKey: "AIzaSyBOWlCGjR0sVdsMYaOJp0N0_8u6VdPZfv8",
  authDomain: "api-weather-2024.firebaseapp.com",
  databaseURL: "https://api-weather-2024-default-rtdb.firebaseio.com",
  projectId: "api-weather-2024",
  storageBucket: "api-weather-2024.appspot.com",
  messagingSenderId: "558915175854",
  appId: "1:558915175854:web:64093f422f898cb304b43f",
  measurementId: "G-1LFNLRS7YC"
};

const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

const medicionesRef = ref(database, "mediciones");

const tempElement = document.getElementById('temp');
const humElement = document.getElementById('hum');
const presElement = document.getElementById('pres');
const windDirElement = document.getElementById('windDir');
const windSpeedElement = document.getElementById('windSpeed');
const raininElement = document.getElementById('rainin');
const battElement = document.getElementById('battlevel');
const lightElement = document.getElementById('lightlevel');
const fechaElement = document.getElementById('fecha');

let mediciones = [];

onValue(medicionesRef, (snapshot) => {
  mediciones = snapshot.val() ? Object.values(snapshot.val()) : [];
  
  if (mediciones.length > 0) {
    const lastMedicion = mediciones[mediciones.length - 1];

    tempElement.textContent = lastMedicion.Temperature;  //temperatura

    humElement.textContent = lastMedicion.Humidity;//humedad

    presElement.textContent = lastMedicion.Pressure;//presion

    windDirElement.textContent = lastMedicion.WindDir; //Direccion del viento

    windSpeedElement.textContent = lastMedicion.WindSpeed; //Velocidad del viento

    raininElement.textContent = lastMedicion.Rain; //Nivel de lluvia

    battElement.textContent = lastMedicion.Batt; //Nivel de bateria

    lightElement.textContent = lastMedicion.Light;

    fechaElement.textContent = lastMedicion.time;

  } else {
    console.log("No data available");
  }
}, (error) => {
  console.error("Error al obtener datos:", error);
});

const modal = document.getElementById('modal');
const modalTitle = document.getElementById('modalTitle');
const modalChart = document.getElementById('modalChart').getContext('2d');
let chart;

function openModal(type) {
  modal.classList.remove('hidden');

  if (chart) {
    chart.destroy();
  }

  let labels = mediciones.map(m => new Date(m.time).toLocaleString());
  let data = [];

  switch (type) {
    case 'temp':
      modalTitle.textContent = 'Gráfica de Temperatura';
      data = mediciones.map(m => m.Temperature);
      break;
    case 'hum':
      modalTitle.textContent = 'Gráfica de Humedad';
      data = mediciones.map(m => m.Humidity);
      break;
    case 'pres':
      modalTitle.textContent = 'Gráfica de Presión';
      data = mediciones.map(m => m.Pressure);
      break;
    case 'windDir':
        modalTitle.textContent = 'Gráfica de Direccion del viento';
        data = mediciones.map(m => m.WindDir);
        break;
    case 'windSpeed':
        modalTitle.textContent = 'Gráfica de Velocidad de viento';
        data = mediciones.map(m => m.WindSpeed);
        break;
    case 'rainin':
        modalTitle.textContent = 'Gráfica de Lluvia';
        data = mediciones.map(m => m.Rain);
        break;
    case 'lightlevel':
        modalTitle.textContent = 'Gráfica de Luz';
        data = mediciones.map(m => m.Light);
        break;
    case 'battlevel':
        modalTitle.textContent = 'Gráfica de Bateria';
        data = mediciones.map(m => m.Batt);
        break;
  }

  chart = new Chart(modalChart, {
    type: 'line',
    data: {
      labels: labels,
      datasets: [{
        label: modalTitle.textContent,
        data: data,
        borderColor: getRandomColor(),
        borderWidth: 1,
        fill: false
      }]
    },
    options: {
      responsive: true,
      scales: {
        x: {
          display: true,
          title: {
            display: true,
            text: 'Tiempo'
          }
        },
        y: {
          display: true,
          title: {
            display: true,
            text: type === 'temp' ? 'Temperatura (°C)' : (type === 'hum' ? 'Humedad (%)' : 'Presión (hPa)' )
          }
        }
      }
    }
  });
}

// Función para generar un color RGB aleatorio
function getRandomColor() {
    const r = Math.floor(Math.random() * 256);
    const g = Math.floor(Math.random() * 256);
    const b = Math.floor(Math.random() * 256);
    return `rgb(${r}, ${g}, ${b})`;
}

function closeModal() {
  modal.classList.add('hidden');
}

// Expose functions to the global scope
window.openModal = openModal;
window.closeModal = closeModal;
