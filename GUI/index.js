const express = require('express');
const multer = require('multer');
const cors = require('cors');
const { exec } = require('child_process');
const fs = require('fs');
const path = require('path');
const os = require('os');

const app = express();
app.use(cors());

// Definir las rutas de los directorios vitales
const uploadsDir = path.join(__dirname, 'uploads');
const outDir = path.join(__dirname, 'out');
const manualesDir = path.join(__dirname, 'docs');

// Verificar y crear los directorios si no existen
function verificarYCrearDirectorios() {
  if (!fs.existsSync(uploadsDir)) {
    fs.mkdirSync(uploadsDir, { recursive: true });
    console.log(`Directorio '${uploadsDir}' creado.`);
  }
  if (!fs.existsSync(outDir)) {
    fs.mkdirSync(outDir, { recursive: true });
    console.log(`Directorio '${outDir}' creado.`);
  }
  if (!fs.existsSync(manualesDir)) {
    fs.mkdirSync(manualesDir, { recursive: true });
    console.log(`Directorio '${manualesDir}' creado.`);
  }
}

// Llamar a la función para verificar y crear los directorios al inicio
verificarYCrearDirectorios();

// Configuración de Multer para la subida de archivos
const storage = multer.diskStorage({
  destination: function (req, file, cb) {
    cb(null, uploadsDir); // Usar la ruta definida para uploads
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
  const nombreEjecutable = os.platform() === 'win32' ? 'compiladorStC.exe' : './compiladorStC.out';
  const rutaTokens = path.join(outDir, 'tokens.json'); // Usar la ruta definida para out
  const rutaErrores = path.join(outDir, 'errores.json'); // Usar la ruta definida para out
  const rutaArbol = path.join(outDir, 'ast.json'); // Usar la ruta definida para out
  const rutaCodigo = path.join(outDir, 'salida.cpp'); // Usar la ruta definida para out

  // Limpiar la carpeta 'out' antes de la compilación
  fs.readdirSync(outDir).forEach(file => {
    const filePath = path.join(outDir, file);
    fs.unlinkSync(filePath);
  });

  // Ejecutar el compilador
  exec(`${nombreEjecutable} "${archivoSubido}"`, (error, stdout, stderr) => {
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
        console.log('No se detectaron errores en el código.');
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
        if (fs.existsSync(rutaArbol)) fs.unlinkSync(rutaArbol);
        if (fs.existsSync(rutaCodigo)) fs.unlinkSync(rutaCodigo);
      } catch (err) {
        console.error('Error al limpiar archivos de salida:', err);
      }
    }, 2000); // Esperar un poco para asegurar que se envíe la respuesta
  });
});

// Ruta para descargar el manual
app.get('/descargar-manual', (req, res) => {
  const manualPath = path.join(manualesDir, 'manual_compilador_stc.pdf'); // Usar la ruta definida para manuales

  fs.access(manualPath, fs.constants.R_OK, (err) => {
    if (err) {
      console.error('No se pudo encontrar o leer el manual:', err);
      return res.status(404).send('Manual no encontrado');
    }

    res.download(manualPath, 'manual_compilador_stc.pdf', (error) => {
      if (error) {
        console.error('Error al descargar el manual:', error);
        return res.status(500).send('Error al descargar el manual');
      }
    });
  });
});

const PORT = 3000;
app.listen(PORT, () => {
  console.log(`Servidor escuchando en el puerto ${PORT}`);
});