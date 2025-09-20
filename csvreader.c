#include <stdio.h>       // for input and output like printf, fopen...
#include <stdlib.h>      // for memory, numbers, and exit like malloc, atoi, exit...
#include <string.h>      // for working with text like strlen, strcpy, strcmp...
#include <json-c/json.h> // for reading and writing JSON files

// this part defines the SmartLog structure.
typedef struct
{
    char device_id[8];
    char timestamp[20];
    float temperature;
    int humidity;
    char status[10];
    char location[31];
    char alert_level[10];
    int battery;
    char firmware_ver[10];
    int event_code;
} SmartLog;

// seperator choosing

char getSeparatorChar(int separator)
{
    if (separator == 1)
        return ',';
    if (separator == 2)
        return '\t';
    if (separator == 3)
        return ';';
    return ',';
}

// removes the end of the lines
void trimNewline(char *str)
{
    size_t len = strlen(str);
    if (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r'))
    {
        str[len - 1] = '\0';
    }
}

// checks firmware version
int isValidFirmware(const char *fw)
{
    int major, minor, patch;
    return sscanf(fw, "v%d.%d.%d", &major, &minor, &patch) == 3;
}

// checks if alert level is one of the valid values
int isValidAlertLevel(const char *lvl)
{
    const char *valid[] = {"LOW", "MEDIUM", "HIGH", "CRITICAL"};
    for (int i = 0; i < 4; i++)
    {
        if (strcmp(lvl, valid[i]) == 0)
            return 1;
    }
    return 0;
}

//-------------------------MAIN-----------------------------

int main(int argc, char *argv[])
{

    // choosing seperator and operating system
    if (argc == 2 && strcmp(argv[1], "-h") == 0)
    {
        printf("Usage: ./2022510069_2023510057_csvreader -separator <1|2|3> -opsys <1|2|3>\n");
        return 0;
    }

    if (argc != 5 || strcmp(argv[1], "-separator") != 0 || strcmp(argv[3], "-opsys") != 0)
    {
        printf("Usage: ./2022510069_2023510057_csvreader -separator <1|2|3> -opsys <1|2|3>\n");
        return 1;
    }

    int separator = atoi(argv[2]);
    int opsys = atoi(argv[4]);

    char sep = getSeparatorChar(separator);

    // opens CSV and binary file for reading and writing
    FILE *csv = fopen("smartlogs.csv", "r");
    if (!csv)
    {
        perror("Cannot opn smartlogs.csv");
        return 1;
    }

    FILE *bin = fopen("logdata.dat", "wb");
    if (!bin)
    {
        perror("Cannot create logdata.dat");
        fclose(csv);
        return 1;
    }

    char line[512];
    fgets(line, sizeof(line), csv); // to skip header

    // reads each line from the CSV and saves data in the SmartLog struct.
    while (fgets(line, sizeof(line), csv))
    {
        trimNewline(line);

        SmartLog log = {0};
        char *token;
        int field = 0;

        token = strtok(line, &sep);
        while (token != NULL)
        {
            switch (field)
            {
            case 0:
                strncpy(log.device_id, token, sizeof(log.device_id) - 1);
                break;
            case 1:
                strncpy(log.timestamp, token, sizeof(log.timestamp) - 1);
                break;
            case 2:
                log.temperature = strlen(token) ? atof(token) : 0.0;
                break;
            case 3:
                log.humidity = strlen(token) ? atoi(token) : 0;
                break;
            case 4:
                strncpy(log.status, token, sizeof(log.status) - 1);
                break;
            case 5:
                strncpy(log.location, token, sizeof(log.location) - 1);
                break;
            case 6:
                strncpy(log.alert_level, token, sizeof(log.alert_level) - 1);
                break;
            case 7:
                log.battery = strlen(token) ? atoi(token) : 0;
                break;
            case 8:
                strncpy(log.firmware_ver, token, sizeof(log.firmware_ver) - 1);
                break;
            case 9:
                log.event_code = strlen(token) ? atoi(token) : 0;
                break;
            }
            field++;
            token = strtok(NULL, &sep);
        }
        // saves each log into the binary file.
        fwrite(&log, sizeof(SmartLog), 1, bin);
    }

    fclose(csv);
    fclose(bin);

    // reads the setupParams.json file and gets sorting info
    FILE *jsonf = fopen("setupParams.json", "r");
    int keyStart = 0, keyEnd = 7;
    char order[10] = "ASC";

    if (jsonf)
    {
        struct json_object *parsed_json;
        struct json_object *json_keyStart, *json_keyEnd, *json_order;

        parsed_json = json_object_from_file("setupParams.json");
        if (parsed_json)
        {
            json_object_object_get_ex(parsed_json, "keyStart", &json_keyStart);
            json_object_object_get_ex(parsed_json, "keyEnd", &json_keyEnd);
            json_object_object_get_ex(parsed_json, "order", &json_order);

            keyStart = json_object_get_int(json_keyStart);
            keyEnd = json_object_get_int(json_keyEnd);
            strncpy(order, json_object_get_string(json_order), sizeof(order) - 1);

            json_object_put(parsed_json);
        }
        fclose(jsonf);
    }
    // reads binary file into memory
    FILE *binRead = fopen("logdata.dat", "rb");
    SmartLog logs[200];
    int logCount = 0;

    while (fread(&logs[logCount], sizeof(SmartLog), 1, binRead))
    {
        logCount++;
    }
    fclose(binRead);

    // sorts the logs
    for (int i = 0; i < logCount - 1; i++)
    {
        for (int j = 0; j < logCount - i - 1; j++)
        {
            char key1[10] = {0}, key2[10] = {0};
            strncpy(key1, logs[j].device_id + keyStart, keyEnd - keyStart);
            strncpy(key2, logs[j + 1].device_id + keyStart, keyEnd - keyStart);

            int cmp = strcmp(key1, key2);
            if ((strcmp(order, "ASC") == 0 && cmp > 0) || (strcmp(order, "DESC") == 0 && cmp < 0))
            {
                SmartLog temp = logs[j];
                logs[j] = logs[j + 1];
                logs[j + 1] = temp;
            }
        }
    }
    // creates an XML file and starts writing data into it
    FILE *xml = fopen("smartlogs.xml", "w");
    if (!xml)
    {
        perror("Cannot create output.xml");
        return 1;
    }

    fprintf(xml, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(xml, "<smartlogs>\n");

    for (int i = 0; i < logCount; i++)
    {
        SmartLog *log = &logs[i];
        fprintf(xml, "  <entry id=\"%d\">\n", i + 1);

        fprintf(xml, "    <device>\n");
        fprintf(xml, "      <device_id>%s</device_id>\n", log->device_id);
        fprintf(xml, "      <location>%s</location>\n", strlen(log->location) ? log->location : "");
        if (isValidFirmware(log->firmware_ver))
            fprintf(xml, "      <firmware_ver>%s</firmware_ver>\n", log->firmware_ver);
        else
            fprintf(xml, "      <firmware_ver></firmware_ver>\n");
        fprintf(xml, "    </device>\n");

        const char *alert;
        if (isValidAlertLevel(log->alert_level))
            alert = log->alert_level;
        else
            alert = "";

        fprintf(xml, "    <metrics status=\"%s\" alert_level=\"%s\">\n", log->status, alert);
        fprintf(xml, "      <temperature>%.2f</temperature>\n", log->temperature);
        fprintf(xml, "      <humidity>%d</humidity>\n", log->humidity);
        fprintf(xml, "      <battery>%d</battery>\n", log->battery);
        fprintf(xml, "    </metrics>\n");

        fprintf(xml, "    <timestamp>%s</timestamp>\n", strlen(log->timestamp) ? log->timestamp : "");

        // calculates Big Endian and Little Endian values
        unsigned int event = log->event_code;
        unsigned char *bytes = (unsigned char *)&event;
        fprintf(xml, "    <event_code hexBig=\"%02X%02X%02X%02X\" hexLittle=\"%02X%02X%02X%02X\" decimal=\"%u\">%d</event_code>\n",
                bytes[3], bytes[2], bytes[1], bytes[0],
                bytes[0], bytes[1], bytes[2], bytes[3],
                (bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24)),
                log->event_code);

        fprintf(xml, "  </entry>\n");
    }

    fprintf(xml, "</smartlogs>\n");
    fclose(xml);

    return 0;
}
