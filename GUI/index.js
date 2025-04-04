const express = require('express');
const multer = require('multer');
const cors = require('cors');
const { exec } = require('child_process');
const fs = require('fs');
const path = require('path');
const os = require('os');

const app = express();
app.use(cors());

// Configuración de Multer para la subida de archivos
const storage = multer.diskStorage({
  destination: function (req, file, cb) {
    cb(null, 'uploads/'); // Directorio donde se guardarán los archivos subidos
  },
  filename: function (req, file, cb) {
    cb(null, file.originalname); // Usar el nombre original del archivo
  }
});

const upload = multer({ storage: storage });

app.post('/compilar', upload.single('archivo'), (req, res) => {
  const archivoSubido = req.file.path;
  const nombreArchivo = req.file.originalname;

  // Determinar el nombre del ejecutable según el sistema operativo
  const nombreEjecutable = os.platform() === 'win32' ? 'compiladorStC.exe' : 'compiladorStC.out';

  // Ejecutar el compilador
  exec(`cd ./out && ${nombreEjecutable} ${archivoSubido}`, (error, stdout, stderr) => {
    if (error) {
      console.error(`Error al ejecutar el compilador: ${error}`);
      return res.status(500).json({ error: 'Error al ejecutar el compilador' });
    }

    // Leer los archivos de salida
    const rutaErrores = path.join(__dirname, 'out', 'errores.json');
    const rutaSimbolos = path.join(__dirname, 'out', 'simbolos.json');
    const rutaArbol = path.join(__dirname, 'out', 'arbol.json');
    const rutaCodigo = path.join(__dirname, 'out', 'codigo.cpp');

    try {
      const errores = JSON.parse(fs.readFileSync(rutaErrores, 'utf8'));
      const simbolos = JSON.parse(fs.readFileSync(rutaSimbolos, 'utf8'));
      const arbol = JSON.parse(fs.readFileSync(rutaArbol, 'utf8'));
      const codigoCompilado = fs.readFileSync(rutaCodigo, 'utf8');

      // Enviar la respuesta al frontend
      res.json({
        errores: errores,
        simbolos: simbolos,
        arbol: arbol,
        codigoCompilado: codigoCompilado
      });
    } catch (err) {
      console.error(`Error al leer los archivos de salida: ${err}`);
      return res.status(500).json({ error: 'Error al leer los archivos de salida' });
    } finally {
      // Eliminar los archivos temporales (opcional)
      fs.unlinkSync(archivoSubido);
      fs.unlinkSync(rutaErrores);
      fs.unlinkSync(rutaSimbolos);
      fs.unlinkSync(rutaArbol);
      fs.unlinkSync(rutaCodigo);
    }
  });
});

const PORT = 3000;
app.listen(PORT, () => {
  console.log(`Servidor escuchando en el puerto ${PORT}`);
});