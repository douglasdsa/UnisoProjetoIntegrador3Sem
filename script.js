let device;

  // Função para mostrar data e hora local atualizada a cada segundo
  function atualizarDataHora() {
    const agora = new Date();
    const dia = String(agora.getDate()).padStart(2, '0');
    const mes = String(agora.getMonth() + 1).padStart(2, '0');
    const ano = agora.getFullYear();
    const horas = String(agora.getHours()).padStart(2, '0');
    const minutos = String(agora.getMinutes()).padStart(2, '0');
    const segundos = String(agora.getSeconds()).padStart(2, '0');
    document.getElementById('dataHora').innerText = `${dia}/${mes}/${ano} ${horas}:${minutos}:${segundos}`;
  }

  setInterval(atualizarDataHora, 1000);
  atualizarDataHora();

  // Mostra uma "tela" e oculta outra
  function mostrarTela(telaId) {
    document.getElementById('conectarSection').classList.remove('active');
    document.getElementById('progCaixa').classList.remove('active');
    document.getElementById(telaId).classList.add('active');
  }

  // Inicia mostrando tela de conexão
  mostrarTela('conectarSection');

  // Botão conectar
  document.getElementById('btnConectar').addEventListener('click', async () => {
    try {
      const options = {
        filters: [{ namePrefix: 'ESP32' }],
        optionalServices: ['4fafc201-1fb5-459e-8fcc-c5c9c331914b']
      };

      device = await navigator.bluetooth.requestDevice(options);

      device.addEventListener('gattserverdisconnected', () => {
        alert('Dispositivo desconectado! Voltando para a tela inicial.');
        mostrarTela('conectarSection');
      });

      const server = await device.gatt.connect();

      document.getElementById('statusProg').innerText = `Conectado ao dispositivo BLE: ${device.name}`;

      mostrarTela('progCaixa');

    } catch (error) {
      document.getElementById('status').innerText = `Erro ao conectar: ${error}`;
    }
  });

  // Botão desconectar
  document.getElementById('btnDesconectar').addEventListener('click', async () => {
    if (device && device.gatt.connected) {
      await device.gatt.disconnect();
      mostrarTela('conectarSection');
      document.getElementById('status').innerText = 'Desconectado manualmente.';
    }
  });