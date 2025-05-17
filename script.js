
let device, characteristic;
const serviceUUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const characteristicUUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

document.getElementById("enviarTempo").addEventListener("click", async () => {
    const tempo = parseInt(document.getElementById("tempo").value);
    if (isNaN(tempo) || tempo <= 0) {
        alert("Informe um tempo válido.");
        return;
    }

    if (!device) {
        try {
            device = await navigator.bluetooth.requestDevice({
                filters: [{ namePrefix: "ESP32" }],
                optionalServices: [serviceUUID]
            });

            device.addEventListener("gattserverdisconnected", () => {
                alert("ESP32 desconectado. Recarregue a página para reconectar.");
                location.reload();
            });

            const server = await device.gatt.connect();
            const service = await server.getPrimaryService(serviceUUID);
            characteristic = await service.getCharacteristic(characteristicUUID);
            document.getElementById("connectionStatus").innerText = "Status: Conectado";

            document.getElementById("setupSection").style.display = "none";
            document.getElementById("acaoSection").style.display = "block";

        } catch (error) {
            alert("Erro ao conectar: " + error);
            return;
        }
    }

    // Enviar o tempo em segundos
    const encoder = new TextEncoder();
    await characteristic.writeValue(encoder.encode("TEMPO:" + tempo));
});

document.getElementById("confirmarBtn").addEventListener("click", async () => {
    if (!characteristic) return;
    const encoder = new TextEncoder();
    await characteristic.writeValue(encoder.encode("CONFIRMAR"));
});
