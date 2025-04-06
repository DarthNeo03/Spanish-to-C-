const { exec } = require("child_process");
const fs = require("fs");

exec("cd ./out && json.exe", (error, stdout, stderr) => {
    if (error) {
        console.error(`Error: ${error.message}`);
        return;
    }
    if (stderr) {
        console.error(`stderr: ${stderr}`);
        return;
    }
    console.log("Compilador ejecutado con éxito");

    // Leer los archivos generados
    const jsonData = fs.readFileSync("./out/salida.json", "utf8");
    const codigoCpp = fs.readFileSync("./out/salida.cpp", "utf8");

    console.log("JSON:", jsonData);
    console.log("Código C++:", codigoCpp);
});
