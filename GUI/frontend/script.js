// Función para procesar los datos recibidos del backend
function procesarDatos(datos) {
    // Mostrar el código compilado
    document.getElementById("codigo-compilado").textContent = datos.codigoCompilado;
  
    // Generar la tabla de símbolos
    const tablaSimbolosElement = document.getElementById("tabla-simbolos");
    tablaSimbolosElement.innerHTML = ""; // Limpiar la tabla
  
    // Crear la fila de encabezado
    const encabezadoSimbolos = document.createElement("tr");
    const encabezadoNombre = document.createElement("th");
    encabezadoNombre.textContent = "Nombre";
    const encabezadoTipo = document.createElement("th");
    encabezadoTipo.textContent = "Tipo";
    const encabezadoValor = document.createElement("th");
    encabezadoValor.textContent = "Valor";
    encabezadoSimbolos.appendChild(encabezadoNombre);
    encabezadoSimbolos.appendChild(encabezadoTipo);
    encabezadoSimbolos.appendChild(encabezadoValor);
    tablaSimbolosElement.appendChild(encabezadoSimbolos);
  
    // Crear las filas de datos
    datos.simbolos.forEach(simbolo => {
      const fila = document.createElement("tr");
      const celdaNombre = document.createElement("td");
      celdaNombre.textContent = simbolo.nombre;
      const celdaTipo = document.createElement("td");
      celdaTipo.textContent = simbolo.tipo;
      const celdaValor = document.createElement("td");
      celdaValor.textContent = simbolo.valor;
      fila.appendChild(celdaNombre);
      fila.appendChild(celdaTipo);
      fila.appendChild(celdaValor);
      tablaSimbolosElement.appendChild(fila);
    });
  
    // Generar la tabla de errores
    const tablaErroresElement = document.getElementById("tabla-errores");
    tablaErroresElement.innerHTML = ""; // Limpiar la tabla
  
    // Crear la fila de encabezado
    const encabezadoErrores = document.createElement("tr");
    const encabezadoLinea = document.createElement("th");
    encabezadoLinea.textContent = "Línea";
    const encabezadoColumna = document.createElement("th");
    encabezadoColumna.textContent = "Columna";
    const encabezadoTipoError = document.createElement("th"); // Cambiar el nombre de la variable aquí
    encabezadoTipoError.textContent = "Tipo";
    const encabezadoDescripcion = document.createElement("th");
    encabezadoDescripcion.textContent = "Descripción";
    encabezadoErrores.appendChild(encabezadoLinea);
    encabezadoErrores.appendChild(encabezadoColumna);
    encabezadoErrores.appendChild(encabezadoTipoError); // Usar el nuevo nombre de la variable aquí
    encabezadoErrores.appendChild(encabezadoDescripcion);
    tablaErroresElement.appendChild(encabezadoErrores);
  
    // Crear las filas de datos
    const errores = datos.errores.tablaErrores || []; // Asegurarse de que errores sea un array
    console.log(errores); // Verificar el contenido de errores
    errores.forEach(error => {
      const fila = document.createElement("tr");
      const celdaLinea = document.createElement("td");
      celdaLinea.textContent = error.linea;
      const celdaColumna = document.createElement("td");
      celdaColumna.textContent = error.columna;
      const celdaTipo = document.createElement("td");
      celdaTipo.textContent = error.tipo;
      const celdaDescripcion = document.createElement("td");
      celdaDescripcion.textContent = error.descripcion;
      fila.appendChild(celdaLinea);
      fila.appendChild(celdaColumna);
      fila.appendChild(celdaTipo);
      fila.appendChild(celdaDescripcion);
      tablaErroresElement.appendChild(fila);
    });
  
    // Generar el árbol sintáctico (aquí puedes usar una librería como jsTree)
    const arbolSintacticoElement = document.getElementById("arbol-sintactico");
    arbolSintacticoElement.innerHTML = ""; // Limpiar el árbol
    // ... (código para generar el árbol sintáctico a partir de datos.arbol)
  }
  
  // Modificar la función para manejar la respuesta del backend
  document.getElementById("form-compilador").addEventListener("submit", function(event) {
    event.preventDefault(); // Evitar la recarga de la página
  
    const archivo = document.getElementById("archivo-codigo").files[0];
    const formData = new FormData();
    formData.append("archivo", archivo);
  
    fetch("http://localhost:3000/compilar", {
      method: "POST",
      body: formData
    })
    .then(response => response.json())
    .then(data => {
      // Procesar los datos para las tablas y el árbol
      procesarDatos(data);
    })
    .catch(error => {
      console.error("Error:", error);
      alert("Ocurrió un error al procesar el archivo.");
    });
  });
  
  // Event listener para cargar el JSON manualmente
  document.getElementById("cargar-json").addEventListener("click", function() {
    const archivoJson = document.getElementById("archivo-json").files[0];
    if (archivoJson) {
      const lector = new FileReader();
      lector.onload = function(evento) {
        try {
          const datosJson = JSON.parse(evento.target.result);
          procesarDatos(datosJson);
        } catch (error) {
          console.error("Error al parsear el JSON:", error);
          alert("Error al cargar o procesar el archivo JSON.");
        }
      };
      lector.readAsText(archivoJson);
    } else {
      alert("Por favor, selecciona un archivo JSON.");
    }
  });