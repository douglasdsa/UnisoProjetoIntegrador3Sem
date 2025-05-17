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

// ===== MedAlert Logic =====
let characteristic;

document.getElementById("enviarTempo")?.addEventListener("click", async () => {
    const tempo = parseInt(document.getElementById("tempo").value);
    if (isNaN(tempo) || tempo <= 0) {
        alert("Informe um tempo válido.");
        return;
    }

    try {
        if (!device || !device.gatt.connected) {
            alert("Conecte-se primeiro ao ESP32.");
            return;
        }

        const service = await device.gatt.getPrimaryService("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
        characteristic = await service.getCharacteristic("beb5483e-36e1-4688-b7f5-ea07361b26a8");

        const encoder = new TextEncoder();
        await characteristic.writeValue(encoder.encode("TEMPO:" + tempo));

        document.getElementById("acaoSection").style.display = "block";
        document.getElementById("ledStatus").innerText = "Aguardando acendimento do LED...";
    } catch (error) {
        console.error("Erro ao enviar tempo:", error);
    }
});

document.getElementById("confirmarBtn")?.addEventListener("click", async () => {
    try {
        if (!characteristic) return;
        const encoder = new TextEncoder();
        await characteristic.writeValue(encoder.encode("CONFIRMAR"));
        document.getElementById("ledStatus").innerText = "Aguardando próximo LED...";
    } catch (error) {
        console.error("Erro ao confirmar:", error);
    }
});

// Integração com fluxo de conexão do ESP32
async function conectarESP32MedAlert() {
    try {
        device = await navigator.bluetooth.requestDevice({
            filters: [{ namePrefix: "ESP32" }],
            optionalServices: ["4fafc201-1fb5-459e-8fcc-c5c9c331914b"]
        });

        device.addEventListener("gattserverdisconnected", () => {
            alert("ESP32 desconectado.");
            document.getElementById("medalert").style.display = "none";
            document.getElementById("setupSection").style.display = "block";
        });

        const server = await device.gatt.connect();
        const service = await server.getPrimaryService("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
        characteristic = await service.getCharacteristic("beb5483e-36e1-4688-b7f5-ea07361b26a8");

        document.getElementById("setupSection").style.display = "none";
        document.getElementById("medalert").style.display = "block";

    } catch (error) {
        console.error("Erro ao conectar ao ESP32:", error);
    }
}

// Suporte para botão de conexão se existir
const conectarBtn = document.getElementById("btnConectarESP32");
if (conectarBtn) {
    conectarBtn.addEventListener("click", conectarESP32MedAlert);
}
