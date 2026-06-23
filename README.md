<p align="center">
  <img src="misc/comodo-virtual-lab_logo.svg" width="500" alt="Logo de COMODO Virtual Lab">
</p>

# COMODO Virtual Lab

Gemelo digital desarrollado en **Unreal Engine 5.7**. Esta aplicación constituye el núcleo de la interfaz de usuario, permitiendo la monitorización y el control de un robot industrial en un entorno 3D.

El proyecto integra el plugin [COMODO Sync](https://github.com/molivass/comodo-sync) para la comunicación con el PLC y visualizar el estado del robot en tiempo real.

## Características Principales

- **Monitorización en Tiempo Real**: Refleja la posición del robot físico.
- **Control Remoto**: Permite controlar el robot con movimientos angulares y lineales mediante una interfaz interactiva.
- **Interfaz de Usuario**: Combina controles 3D (Gizmos custom) y controles 2D (sliders, botones).
- **Cinemática Inversa**: Implementa un solver de cinemática inversa basado en Denavit-Hartenberg.

## Instalación

Se puede encontrar una versión compilada en la pestaña de [Releases](https://github.com/molivass/comodo-virtual-lab/releases)

Si prefieres descargar el proyecto de Unreal:

1.  Clona este repositorio.
2.  Asegúrate de tener instalado **Unreal Engine 5** (Última vez probado en la versión 5.7.1).
3.  Ejecuta el archivo del proyecto directamente, o desde la ventana de inicio del motor.

## Uso

1.  **Conectar**: Pulsa el botón de conexión en la interfaz para enlazar con el servidor .
2.  **Monitorización**: Al conectarte, el modelo reflejará la posición del robot real.
3.  **Control**: Activa el modo de control para manipular el modelo virtual.
4.  **Interacción**: Puedes arrastrar los Gizmos 3D sobre el robot o usar los deslizadores de la interfaz para mover cada eje.
5.  **Enviar Comando**: Pulsa el botón central para que el robot real ejecute el movimiento.