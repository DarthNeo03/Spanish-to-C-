function mostrarTabla(idTabla, datosTabla) {
  const tablaElement = document.getElementById(idTabla);
  tablaElement.innerHTML = "";

  if (!datosTabla || datosTabla.length === 0) {
    const mensajeFila = document.createElement("tr");
    const mensajeCelda = document.createElement("td");
    mensajeCelda.textContent = "No hay datos para mostrar.";
    mensajeCelda.colSpan = 10; // Ajustar según el número máximo de columnas esperado
    mensajeFila.appendChild(mensajeCelda);
    tablaElement.appendChild(mensajeFila);
    return;
  }

  const encabezadoFila = document.createElement("tr");
  const primerElemento = datosTabla[0];
  for (const key in primerElemento) {
    if (Object.hasOwnProperty.call(primerElemento, key)) {
      const th = document.createElement("th");
      th.textContent = key.charAt(0).toUpperCase() + key.slice(1); // Capitalizar encabezados
      encabezadoFila.appendChild(th);
    }
  }
  tablaElement.appendChild(encabezadoFila);

  datosTabla.forEach(item => {
    const dataFila = document.createElement("tr");
    for (const key in item) {
      if (Object.hasOwnProperty.call(item, key)) {
        const td = document.createElement("td");
        td.textContent = item[key];
        dataFila.appendChild(td);
      }
    }
    tablaElement.appendChild(dataFila);
  });
}

function mostrarArbolSintactico(arbol) {
  const arbolSintacticoElement = document.getElementById("arbol-sintactico");
  arbolSintacticoElement.innerHTML = "";
  if (!arbol) {
    arbolSintacticoElement.textContent = "El árbol sintáctico no se generó debido a errores en el código.";
    return;
  }

  function crearNodoArbol(nodo) {
    const nodoDiv = document.createElement("div");
    nodoDiv.classList.add("nodo-arbol");
    nodoDiv.textContent = nodo.nodo || nodo.tipo || nodo.token || "Nodo";
    if (nodo.nombre) nodoDiv.textContent += ` (${nodo.nombre})`;
    if (nodo.valor) nodoDiv.textContent += `: ${nodo.valor}`;
    if (nodo.condicion) nodoDiv.textContent += ` (${nodo.condicion})`;

    if (nodo.hijos && nodo.hijos.length > 0) {
      const hijosDiv = document.createElement("div");
      hijosDiv.classList.add("hijos-arbol");
      nodo.hijos.forEach(hijo => {
        const linea = document.createElement("div");
        linea.classList.add("linea-arbol");
        const hijoNodo = crearNodoArbol(hijo);
        hijosDiv.appendChild(linea);
        hijosDiv.appendChild(hijoNodo);
      });
      nodoDiv.appendChild(hijosDiv);
    }
    return nodoDiv;
  }

  if (arbol) {
    const raizNodo = crearNodoArbol(arbol);
    arbolSintacticoElement.appendChild(raizNodo);
  } else {
    arbolSintacticoElement.textContent = "El árbol sintáctico no se generó.";
  }
}

// Modificar la función para manejar la respuesta del backend
document.getElementById("form-compilador").addEventListener("submit", function(event) {
  event.preventDefault();

  const archivo = document.getElementById("archivo-codigo").files[0];
  const formData = new FormData();
  formData.append("archivo", archivo);

  fetch("http://localhost:3000/compilar", {
    method: "POST",
    body: formData
  })
  .then(response => response.json())
  .then(data => {
    document.getElementById("codigo-compilado").textContent = data.codigoCompilado || "";
    mostrarTabla("tabla-tokens", data.tablaTokens);
    mostrarTabla("tabla-errores", data.tablaErrores);
    mostrarArbolSintactico(data.arbol);

    if (data.compiladorError) {
      alert("Error del compilador: " + data.compiladorError);
    } else if (data.tablaErrores && data.tablaErrores.length > 0) {
      alert("Se encontraron errores en el código.");
    }
  })
  .catch(error => {
    console.error("Error en la petición:", error);
    alert("Ocurrió un error al comunicarse con el servidor.");
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
        document.getElementById("codigo-compilado").textContent = datosJson.codigoCompilado || "";
        mostrarTabla("tabla-tokens", datosJson.tablaTokens);
        mostrarTabla("tabla-errores", datosJson.tablaErrores);
        mostrarArbolSintactico(datosJson.arbol);
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