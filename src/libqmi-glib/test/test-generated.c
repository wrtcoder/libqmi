/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details:
 *
 * Copyright (C) 2014 Aleksander Morgado <aleksander@aleksander.es>
 */

#include <config.h>
#include <libqmi-glib.h>

#include "test-fixture.h"

/*****************************************************************************/

static void
test_generated_core (TestFixture *fixture)
{
    /* Noop */
}

/*****************************************************************************/
/* DMS Get IDs */

static void
dms_get_ids_ready (QmiClientDms *client,
                   GAsyncResult *res,
                   TestFixture  *fixture)
{
    QmiMessageDmsGetIdsOutput *output;
    GError *error = NULL;
    gboolean st;
    const gchar *str;

    output = qmi_client_dms_get_ids_finish (client, res, &error);
    g_assert_no_error (error);
    g_assert (output);

    st = qmi_message_dms_get_ids_output_get_result (output, &error);
    g_assert_no_error (error);
    g_assert (st);

    /* [/dev/cdc-wdm3] Device IDs retrieved:
     * 	 ESN: '80997874'
     * 	IMEI: '359225050039973'
     * 	MEID: '35922505003997' */
    st = qmi_message_dms_get_ids_output_get_esn (output, &str, &error);
    g_assert_no_error (error);
    g_assert (st);
    g_assert_cmpstr (str, ==, "80997874");

    st = qmi_message_dms_get_ids_output_get_imei (output, &str, &error);
    g_assert_no_error (error);
    g_assert (st);
    g_assert_cmpstr (str, ==, "359225050039973");

    st = qmi_message_dms_get_ids_output_get_meid (output, &str, &error);
    g_assert_no_error (error);
    g_assert (st);
    g_assert_cmpstr (str, ==, "35922505003997");

    qmi_message_dms_get_ids_output_unref (output);

    test_fixture_loop_stop (fixture);
}

static void
test_generated_dms_get_ids (TestFixture *fixture)
{
    guint8 expected[] = {
        0x01,
        0x0C, 0x00, 0x00, 0x02, 0x01,
        0x00, 0xFF, 0xFF, 0x25, 0x00, 0x00, 0x00
    };
    guint8 response[] = {
        0x01,
        0x45, 0x00, 0x80, 0x02, 0x01,
        0x02, 0xFF, 0xFF, 0x25, 0x00, 0x39, 0x00, 0x02,
        0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x01,
        0x00, 0x42, 0x12, 0x0E, 0x00, 0x33, 0x35, 0x39,
        0x32, 0x32, 0x35, 0x30, 0x35, 0x30, 0x30, 0x33,
        0x39, 0x39, 0x37, 0x10, 0x08, 0x00, 0x38, 0x30,
        0x39, 0x39, 0x37, 0x38, 0x37, 0x34, 0x11, 0x0F,
        0x00, 0x33, 0x35, 0x39, 0x32, 0x32, 0x35, 0x30,
        0x35, 0x30, 0x30, 0x33, 0x39, 0x39, 0x37, 0x33
    };

    test_port_context_set_command (fixture->ctx,
                                   expected, G_N_ELEMENTS (expected),
                                   response, G_N_ELEMENTS (response),
                                   fixture->service_info[QMI_SERVICE_DMS].transaction_id++);

    qmi_client_dms_get_ids (QMI_CLIENT_DMS (fixture->service_info[QMI_SERVICE_DMS].client), NULL, 3, NULL,
                            (GAsyncReadyCallback) dms_get_ids_ready,
                            fixture);
    test_fixture_loop_run (fixture);
}

/*****************************************************************************/
/* DMS UIM Get PIN Status */

static void
dms_uim_get_pin_status_ready (QmiClientDms *client,
                              GAsyncResult *res,
                              TestFixture  *fixture)
{
    QmiMessageDmsUimGetPinStatusOutput *output;
    GError *error = NULL;
    gboolean st;
    QmiDmsUimPinStatus current_status;
    guint8 verify_retries_left;
    guint8 unblock_retries_left;

    output = qmi_client_dms_uim_get_pin_status_finish (client, res, &error);
    g_assert_no_error (error);
    g_assert (output);

    st = qmi_message_dms_uim_get_pin_status_output_get_result (output, &error);
    g_assert_no_error (error);
    g_assert (st);

    /* [/dev/cdc-wdm3] PIN1:
     *   Status: enabled-not-verified
     *   Verify: 3
     *  Unblock: 10
     * [/dev/cdc-wdm3] PIN2:
     *   Status: enabled-not-verified
     *   Verify: 2
     *  Unblock: 10
     */

    st = (qmi_message_dms_uim_get_pin_status_output_get_pin1_status (
              output,
              &current_status,
              &verify_retries_left,
              &unblock_retries_left,
              &error));
    g_assert_no_error (error);
    g_assert (st);
    g_assert_cmpuint (current_status, ==, QMI_DMS_UIM_PIN_STATUS_ENABLED_NOT_VERIFIED);
    g_assert_cmpuint (verify_retries_left, ==, 3);
    g_assert_cmpuint (unblock_retries_left, ==, 10);

    st = (qmi_message_dms_uim_get_pin_status_output_get_pin2_status (
              output,
              &current_status,
              &verify_retries_left,
              &unblock_retries_left,
              &error));
    g_assert_no_error (error);
    g_assert (st);
    g_assert_cmpuint (current_status, ==, QMI_DMS_UIM_PIN_STATUS_ENABLED_NOT_VERIFIED);
    g_assert_cmpuint (verify_retries_left, ==, 2);
    g_assert_cmpuint (unblock_retries_left, ==, 10);

    qmi_message_dms_uim_get_pin_status_output_unref (output);

    test_fixture_loop_stop (fixture);
}

static void
test_generated_dms_uim_get_pin_status (TestFixture *fixture)
{
    guint8 expected[] = {
        0x01,
        0x0C, 0x00, 0x00, 0x02, 0x01,
        0x00, 0xFF, 0xFF, 0x2B, 0x00, 0x00, 0x00
    };
    guint8 response[] = {
        0x01,
        0x1F, 0x00, 0x80, 0x02, 0x01,
        0x02, 0xFF, 0xFF, 0x2B, 0x00, 0x13, 0x00, 0x02,
        0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x03,
        0x00, 0x01, 0x02, 0x0A, 0x11, 0x03, 0x00, 0x01,
        0x03, 0x0A
    };

    test_port_context_set_command (fixture->ctx,
                                   expected, G_N_ELEMENTS (expected),
                                   response, G_N_ELEMENTS (response),
                                   fixture->service_info[QMI_SERVICE_DMS].transaction_id++);

    qmi_client_dms_uim_get_pin_status (QMI_CLIENT_DMS (fixture->service_info[QMI_SERVICE_DMS].client), NULL, 3, NULL,
                                       (GAsyncReadyCallback) dms_uim_get_pin_status_ready,
                                       fixture);
    test_fixture_loop_run (fixture);
}

/*****************************************************************************/

int main (int argc, char **argv)
{
    g_type_init ();
    g_test_init (&argc, &argv, NULL);

    /* Test the setup/teardown test methods */
    TEST_ADD ("/libqmi-glib/generated/core", test_generated_core);

    /* DMS */
    TEST_ADD ("/libqmi-glib/generated/dms/get-ids",            test_generated_dms_get_ids);
    TEST_ADD ("/libqmi-glib/generated/dms/uim-get-pin-status", test_generated_dms_uim_get_pin_status);


    return g_test_run ();
}