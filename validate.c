#include <stdio.h>
#include <libxml/xmlschemastypes.h>

int main()
{
    xmlDocPtr doc;
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;

    char *XMLFileName = "smartlogs.xml";
    char *XSDFileName = "2022510069_2023510057.xsd";
    
    xmlLineNumbersDefault(1);
    ctxt = xmlSchemaNewParserCtxt(XSDFileName);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

    doc = xmlReadFile(XMLFileName, NULL, 0);
    if (doc == NULL)
    {
        fprintf(stderr, "Could not parse %s\n", XMLFileName);
    }
    else
    {
        xmlSchemaValidCtxtPtr ctxt;
        int ret;

        ctxt = xmlSchemaNewValidCtxt(schema);
        ret = xmlSchemaValidateDoc(ctxt, doc);
        if (ret == 0)
        {
            printf("%s validates\n", XMLFileName);
        }
        else if (ret > 0)
        {
            printf("%s fails to validate\n", XMLFileName);
        }
        else
        {
            printf("%s validation generated an internal error\n", XMLFileName);
        }
        xmlSchemaFreeValidCtxt(ctxt);
        xmlFreeDoc(doc);
    }

    if(schema != NULL)
        xmlSchemaFree(schema);

    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();
    return(0);
}
