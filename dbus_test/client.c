#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>

int main() {
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *msg = NULL;
    sd_bus *bus = NULL;
    sd_bus_error reply_error = SD_BUS_ERROR_NULL;
    sd_bus_message *reply = NULL;
    int result = 0;

    // D-Bus-Verbindung initialisieren
    int ret = sd_bus_open_system(&bus);
    if (ret < 0) {
        fprintf(stderr, "Fehler beim Öffnen der D-Bus-Verbindung: %s\n", strerror(-ret));
        goto finish;
    }

    // Den Dienstnamen des Anbieters angeben
    const char *service = "com.example.ExampleService";
    const char *object_path = "/com/example/ExampleObject";
    const char *interface = "com.example.ExampleInterface";
    const char *method = "Square";

    // D-Bus-Nachricht erstellen
    ret = sd_bus_message_new_method_call(bus, &msg, service, object_path, interface, method);
    if (ret < 0) {
        fprintf(stderr, "Fehler beim Erstellen der D-Bus-Nachricht: %s\n", strerror(-ret));
        goto finish;
    }

    // Eingabeparameter für die Funktion festlegen
    int num = 5;
    ret = sd_bus_message_append_basic(msg, 'i', &num);
    if (ret < 0) {
        fprintf(stderr, "Fehler beim Hinzufügen der Eingabeparameter zur D-Bus-Nachricht: %s\n", strerror(-ret));
        goto finish;
    }

    // Die D-Bus-Nachricht senden und auf die Antwort warten
    ret = sd_bus_call(bus, msg, 0, &error, &reply);
    if (ret < 0) {
        fprintf(stderr, "Fehler beim Senden der D-Bus-Nachricht: %s\n", error.message);
        goto finish;
    }

    // Antwortnachricht verarbeiten und die Antwort extrahieren
    ret = sd_bus_message_read(reply, "i", &result);
    if (ret < 0) {
        fprintf(stderr, "Fehler beim Lesen der Antwort aus der D-Bus-Nachricht: %s\n", strerror(-ret));
        goto finish;
    }

    // Die Antwort ausgeben
    printf("Das Quadrat von %d ist %d\n", num, result);

finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(msg);
    sd_bus_error_free(&reply_error);
    sd_bus_message_unref(reply);
    sd_bus_unref(bus);

    return ret < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}