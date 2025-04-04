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
  const nombreBase = path.basename(nombreArchivo, path.extname(nombreArchivo));

  // Determinar el nombre del ejecutable según el sistema operativo
  const nombreEjecutable = os.platform() === 'win32' ? 'compiladorStC.exe' : 'compiladorStC.out';
  const rutaOut = path.join(__dirname, 'out');
  const rutaTokens = path.join(rutaOut, 'tokens.json');
  const rutaErrores = path.join(rutaOut, 'errores.json');
  const rutaArbol = path.join(rutaOut, 'arbol.json');
  const rutaCodigo = path.join(rutaOut, 'salida.cpp');

  // Asegurar que la carpeta 'out' exista
  if (!fs.existsSync(rutaOut)) {
    fs.mkdirSync(rutaOut, { recursive: true });
  }

  // Limpiar la carpeta 'out' antes de la compilación
  /*fs.readdirSync(rutaOut).forEach(file => {
    const filePath = path.join(rutaOut, file);
    fs.unlinkSync(filePath);
  });*/

  // Ejecutar el compilador
  exec(`${nombreEjecutable} ./uploads/${nombreArchivo}`, (error, stdout, stderr) => {
    // Eliminar el archivo subido
    fs.unlinkSync(archivoSubido);

    let respuesta = {};
    let huboError = false;

    // Intentar leer la tabla de tokens
    try {
      const tTokens = JSON.parse(fs.readFileSync(rutaTokens, 'utf8'));
      respuesta.tablaTokens = tTokens.tablaTokens;
      //console.log('Tabla de tokens:', respuesta.tablaTokens);
    } catch (err) {
      console.log('No se pudo leer tabla de tokens:', err.message);
    }

    // Intentar leer la tabla de errores
    try {
      if (fs.existsSync(rutaErrores)) {
        const tErrores =  JSON.parse(fs.readFileSync(rutaErrores, 'utf8'));
        respuesta.tablaErrores = tErrores.tablaErrores;
        if (tErrores.tablaErrores && tErrores.tablaErrores.length > 0) {
          huboError = true;
        }
      } else {
        huboError = false;
      }
    } catch (err) {
      console.log('No se pudo leer tabla de errores:', err.message);
    }

    if (!huboError) {
      // Intentar leer el árbol sintáctico
      try {
        respuesta.arbol = JSON.parse(fs.readFileSync(rutaArbol, 'utf8'));
      } catch (err) {
        console.log('No se pudo leer árbol sintáctico:', err.message);
      }

      // Intentar leer el código compilado
      try {
        respuesta.codigoCompilado = fs.readFileSync(rutaCodigo, 'utf8');
      } catch (err) {
        console.log('No se pudo leer código compilado:', err.message);
      }
    }

    if (error) {
      console.error(`Error del compilador (ejecución): ${error}`);
      respuesta.compiladorError = 'Error interno del compilador.';
      return res.status(500).json(respuesta);
    }
    if (stderr) {
      console.error(`Error del compilador (salida estándar de errores): ${stderr}`);
      respuesta.compiladorError = stderr;
      return res.status(200).json(respuesta); // Aun así enviamos lo que se pudo generar
    }

    res.json(respuesta);

    // Limpiar los archivos de salida después de enviar la respuesta
    setTimeout(() => {
      try {
        if (fs.existsSync(rutaTokens)) fs.unlinkSync(rutaTokens);
        if (fs.existsSync(rutaErrores)) fs.unlinkSync(rutaErrores);
        //if (fs.existsSync(rutaArbol)) fs.unlinkSync(rutaArbol);
        if (fs.existsSync(rutaCodigo)) fs.unlinkSync(rutaCodigo);
      } catch (err) {
        console.error('Error al limpiar archivos de salida:', err);
      }
    }, 100); // Esperar un poco para asegurar que se envíe la respuesta
  });
});

const PORT = 3000;
app.listen(PORT, () => {
  console.log(`Servidor escuchando en el puerto ${PORT}`);
});