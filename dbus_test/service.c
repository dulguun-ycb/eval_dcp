#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>
#include <string.h>

// Die Funktion, die über D-Bus aufgerufen werden kann
// Die Funktion, die über D-Bus aufgerufen werden kann
int square(sd_bus_message *msg, void *userdata, sd_bus_error *ret_error) {
    // Argumente aus der D-Bus-Nachricht extrahieren
    int num = 0;
    int ret = sd_bus_message_read(msg, "i", &num);
    if (ret < 0) {
        fprintf(stderr, "Fehler beim Lesen der Argumente: %s\n", strerror(-ret));
        return ret;
    }

    // Quadrat berechnen
    int result = num * num;

    // Antwortnachricht erstellen und das Ergebnis hinzufügen
    sd_bus_message *reply = NULL;
    ret = sd_bus_message_new_method_return(msg, &reply);
    if (ret < 0) {
        fprintf(stderr, "Fehler beim Erstellen der Antwortnachricht: %s\n", strerror(-ret));
        return ret;
    }

    ret = sd_bus_message_append_basic(reply, 'i', &result);
    if (ret < 0) {
        fprintf(stderr, "Fehler beim Hinzufügen der Antwort: %s\n", strerror(-ret));
        return ret;
    }

    // Antwortnachricht senden
    ret = sd_bus_send(NULL, reply, NULL);
    if (ret < 0) {
        fprintf(stderr, "Fehler beim Senden der Antwort: %s\n", strerror(-ret));
        return ret;
    }

    // Antwortnachricht freigeben
    sd_bus_message_unref(reply);

    return ret;
}

void setup(){
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_slot *slot = NULL;
    sd_bus *bus = NULL;

    // D-Bus-Verbindung initialisieren
    int ret = sd_bus_open_system(&bus); 
    if (ret < 0) {
        fprintf(stderr, "Fehler beim Öffnen der D-Bus-Verbindung: %s\n", strerror(-ret));
        goto finish;
    }

    // D-Bus-Vtable definieren
    static const sd_bus_vtable vtable[] = {
        SD_BUS_VTABLE_START(0),
        SD_BUS_METHOD("Square", "i", "i", square, SD_BUS_VTABLE_UNPRIVILEGED),
        SD_BUS_VTABLE_END
    };

    // Die Methode registrieren, die über D-Bus aufgerufen werden kann
    ret = sd_bus_add_object_vtable(bus, &slot, "/com/example/ExampleObject", "com.example.ExampleInterface", vtable, NULL);
    if (ret < 0) {
        fprintf(stderr, "Fehler beim Registrieren des Objekts: %s\n", strerror(-ret));
        goto finish;
    }

    // Den Dienstnamen bekannt geben
    ret = sd_bus_request_name(bus, "com.example.ExampleService", 0);
    if (ret < 0) {
        fprintf(stderr, "Fehler beim Beantragen des Dienstnamens: %s\n", strerror(-ret));
        goto finish;
    }
    // Endlosschleife für die Verarbeitung von D-Bus-Nachrichten
    printf("DEBUG: vor while\n");

    while (1) {
        // Auf D-Bus-Nachrichten warten und darauf reagieren
        ret = sd_bus_process(bus, NULL);
        if (ret < 0) {
            fprintf(stderr, "Fehler beim Verarbeiten der D-Bus-Nachrichten: %s\n", strerror(-ret));
            goto finish;
        }
    }

finish:
    sd_bus_error_free(&error);
    sd_bus_slot_unref(slot);
    sd_bus_unref(bus);
}

int main() {

    setup();
    return 0;

}