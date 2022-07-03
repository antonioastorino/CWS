#include "class_http.h"
#include "class_string_array.h"

Error HttpReqObj_new(const char* raw_request, HttpReqObj* out_http_req_obj)
{
    // Split the raw data into header and body.
    StringArray raw_data_string_array_obj = StringArray_new(raw_request, "\r\n\r\n");
    LOG_INFO("Elements in the array: `%lu`", raw_data_string_array_obj.num_of_elements);
    if (raw_data_string_array_obj.num_of_elements < 2)
    {
        // StringArray_new() returns 0 elements if the string is empty, one element (the original
        // string) if the pattern is not found. Both cases are unacceptable.
        LOG_ERROR("Invalid header");
        StringArray_destroy(&raw_data_string_array_obj);
        return ERR_INVALID;
    }
    // Process the first half, assuming that it's the header.
    char* header  = raw_data_string_array_obj.str_array_char_p[0];
    char* body    = raw_data_string_array_obj.str_array_char_p[1];
    Error ret_res = http_header_init(header, &out_http_req_obj->req_header);
    if (ret_res != ERR_ALL_GOOD)
    {
        LOG_ERROR("Failed to initialize header");
    }
    else
    {
        out_http_req_obj->req_body_string_obj = String_new(body);
    }
    StringArray_destroy(&raw_data_string_array_obj);
    return ERR_ALL_GOOD;
}

Error HttpReqObj_destroy(HttpReqObj* http_req_obj_p)
{
    if (http_req_obj_p == NULL)
    {
        return ERR_NULL;
    }
    String_destroy(&http_req_obj_p->req_body_string_obj);
    return ERR_ALL_GOOD;
}

#if TEST == 1
void test_class_http()
{
    PRINT_BANNER();
    PRINT_TEST_TITLE("Initialize request");
    {
        HttpReqObj req_obj;
        const char* req_raw
            = "POST /some/path PROTOCOL\r\ncontent-type: some_value\r\n\r\nsome body\r\n";
        ASSERT(HttpReqObj_new(req_raw, &req_obj) == ERR_ALL_GOOD, "Valid request.");
        ASSERT(req_obj.req_header.req_method == METHOD_POST, "Method POST correct.");
        ASSERT_EQ(req_obj.req_header.req_path, "/some/path", "Path stored properly.");
        ASSERT(req_obj.req_header.req_protocol == PROTOCOL_VALID, "Protocol correct.");
        ASSERT_EQ(req_obj.req_body_string_obj.str, "some body\r\n", "Body correct");
        HttpReqObj_destroy(&req_obj);
    }
}
#endif /* TEST == 1 */
