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

  // Crear un contenedor para el árbol (necesario para algunas librerías)
  const treeContainer = document.createElement('div');
  arbolSintacticoElement.appendChild(treeContainer);

  // Usando la librería jsTree (asegúrate de incluirla en tu HTML)
  $(treeContainer).jstree({
    core: {
      data: [convertirFormatoArbol(arbol)],
      themes: {
        dots: true,
        icons: false
      },
      animation: 200
    }
  });
}

function convertirFormatoArbol(nodo) {
  let textoNodo = nodo.tipo || 'Nodo';
  const children = [];

  if (nodo.identificador) textoNodo += ` (${nodo.identificador})`;
  if (nodo.tipoDato) textoNodo += `: ${nodo.tipoDato}`;
  if (nodo.valor) textoNodo += ` = ${nodo.valor}`;
  if (nodo.nombre) textoNodo += `: ${nodo.nombre}`;
  if (nodo.argumentos) textoNodo += ` (${nodo.argumentos ? nodo.argumentos.join(', ') : ''})`;
  if (nodo.condicion) textoNodo += ` (${nodo.condicion})`;

  if (nodo.declaraciones) {
    nodo.declaraciones.forEach(decl => children.push(convertirFormatoArbol(decl)));
  }
  if (nodo.instrucciones) {
    nodo.instrucciones.forEach(inst => children.push(convertirFormatoArbol(inst)));
  }
  if (nodo.hijos) {
    nodo.hijos.forEach(child => children.push(convertirFormatoArbol(child)));
  }

  return {
    text: textoNodo,
    children: children
  };
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