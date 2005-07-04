#define	__ASN_INTERNAL_TEST_MODE__
#include <GeneralizedTime.c>
#include <constraints.c>

static void
recognize(char *time_str, time_t expect, int as_gmt) {
	GeneralizedTime_t gt;
	struct tm tm;
	time_t tloc;
	long fv, fb;

	gt.buf = (uint8_t *)time_str;
	gt.size = strlen(time_str);

	tloc = asn_GT2time_frac(&gt, &fv, &fb, &tm, as_gmt);
	printf("%s: [%s] -> %ld == %ld\n",
		as_gmt?"GMT":"ofs", time_str, (long)tloc, (long)expect);

	if(tloc != -1) {
		printf("\t%04d-%02d-%02dT%02d:%02d:%02d(.%ld/%ld)%+03ld%02ld\n",
		tm.tm_year + 1900,
		tm.tm_mon + 1,
		tm.tm_mday,
		tm.tm_hour,
		tm.tm_min,
		tm.tm_sec,
		fv, fb,
		(GMTOFF(tm) / 3600),
		labs(GMTOFF(tm) % 3600)
		);
		assert(fb < 100 || (fb % 100) == 0);
	}
	assert(tloc == expect);

#ifdef	HAVE_TM_GMTOFF
	assert(tloc == -1 || as_gmt == 0 || GMTOFF(tm) == 0);
#endif

	if(!as_gmt) recognize(time_str, expect, 1);
}

static void
encode(time_t tloc, const char *expect, int force_gmt) {
	GeneralizedTime_t *gt;
	struct tm tm, *tmp;

	tmp = localtime_r(&tloc, &tm);
	assert(tmp);

	gt = asn_time2GT(0, &tm, force_gmt);
	if(gt) {
		assert(expect);
		printf("[%s] vs [%s] (%d)\n",
			gt->buf, expect, force_gmt);
		assert(gt->size == (int)strlen((char *)gt->buf));
		assert(!strcmp((char *)gt->buf, expect));
	} else {
		assert(!expect);
	}
}


static void
recode(char *time_str, const char *expect) {
	long frac_value, frac_base;
	GeneralizedTime_t gt;
	struct tm tm;
	time_t tloc;

	gt.buf = (uint8_t *)time_str;
	gt.size = strlen(time_str);

	tloc = asn_GT2time_frac(&gt, &frac_value, &frac_base, &tm, 1);
	assert(tloc != -1);

	gt.buf = 0;
	asn_time2GT_frac(&gt, &tm, frac_value, frac_base, 1);
	assert(gt.buf);

	printf("[%s] => [%s] == [%s]\n", time_str, gt.buf, expect);

	assert(strcmp((char *)gt.buf, expect) == 0);
	FREEMEM(gt.buf);
}

static void
check_fractions() {
	GeneralizedTime_t *gt = 0;
	struct tm tm;

	memset(&tm, 0, sizeof tm);
	tm.tm_year = 70;
	tm.tm_mday = 1;

	gt = asn_time2GT_frac(gt, &tm, -1, -1, 1);
	assert(gt);
	printf("[%s]\n", gt->buf);
	assert(strcmp((char *)gt->buf, "19700101000000Z") == 0);

	gt = asn_time2GT_frac(gt, &tm, 0, 0, 1);
	assert(gt);
	printf("[%s]\n", gt->buf);
	assert(strcmp((char *)gt->buf, "19700101000000Z") == 0);

	gt = asn_time2GT_frac(gt, &tm, 0, -1, 1);
	assert(gt);
	printf("[%s]\n", gt->buf);
	assert(strcmp((char *)gt->buf, "19700101000000Z") == 0);

	gt = asn_time2GT_frac(gt, &tm, -1, 0, 1);
	assert(gt);
	printf("[%s]\n", gt->buf);
	assert(strcmp((char *)gt->buf, "19700101000000Z") == 0);

	gt = asn_time2GT_frac(gt, &tm, 10, 0, 1);
	assert(gt);
	printf("[%s]\n", gt->buf);
	assert(strcmp((char *)gt->buf, "19700101000000Z") == 0);

	/* Normalization should happen prior calling the _frac() */
	gt = asn_time2GT_frac(gt, &tm, 55, 10, 1);
	assert(gt);
	printf("[%s]\n", gt->buf);
	assert(strcmp((char *)gt->buf, "19700101000000Z") == 0);

	gt = asn_time2GT_frac(gt, &tm, 10, 20, 1);
	assert(gt);
	printf("[%s]\n", gt->buf);
	assert(strcmp((char *)gt->buf, "19700101000000.5Z") == 0);

	gt = asn_time2GT_frac(gt, &tm, -10, 20, 1);
	assert(gt);
	printf("[%s]\n", gt->buf);
	assert(strcmp((char *)gt->buf, "19700101000000Z") == 0);

	FREEMEM(gt->buf);
	FREEMEM(gt);
}

int
main(int ac, char **av) {

	(void)av;

	check_fractions();

	recognize("200401250", -1, 0);
	recognize("2004012509300", -1, 0);
	recognize("20040125093000-", -1, 0);
	recognize("20040125093007-0", -1, 0);
	recognize("20040125093007-080", -1, 0);
	recognize("200401250930.01Z", -1, 0);

	/* These six are from X.690:11.7.5 */
	recognize("19920520240000Z", -1, 0);  /* midnight represented incorrectly */
	recognize("19920622123421.0Z", 709216461, 0);  /* spurious trailing zeros */
	recognize("19920722132100.30Z", 711811260, 0); /* spurious trailing zeros */
	recognize("19920521000000Z", 706406400, 0);
	recognize("19920622123421Z", 709216461, 0);
	recognize("19920722132100.3Z", 711811260, 0);

	recognize("20040125093007Z", 1075023007, 0);
	recognize("20040125093007+00", 1075023007, 0);
	recognize("20040125093007.01+0000", 1075023007, 0);
	recognize("20040125093007,1+0000", 1075023007, 0);
	recognize("20040125093007-0800", 1075051807, 0);

	recognize("19920722132100.123000123Z", 711811260, 0);
	recognize("19920722132100.1230000123Z", 711811260, 0);
	recognize("19920722132100.12300000123Z", 711811260, 0);

	encode(1075023007, "20040125093007Z", 1);

	if(ac > 1) {
		/* These will be valid only inside PST time zone */
		recognize("20040125093007", 1075051807, 0);
		recognize("200401250930", 1075051800, 0);
		recognize("20040125093000,01", 1075051800, 0);
		recognize("20040125093000,1234", 1075051800, 0);

		encode(1075023007, "20040125013007-0800", 0);
		recode("20050702123312", "20050702193312Z");
	}

	recode("20050702123312Z", "20050702123312Z");
	recode("20050702123312+01", "20050702113312Z");
	recode("20050702123312,0+01", "20050702113312Z");
	recode("20050702123312,1+01", "20050702113312.1Z");
	recode("20050702123312.01+01", "20050702113312.01Z");
	recode("20050702123312.00+01", "20050702113312Z");
	recode("20050702123312.30+01", "20050702113312.3Z");
	recode("20050702123312,30000+01", "20050702113312.3Z");
	recode("20050702123312,300000000+01", "20050702113312.3Z");
	recode("20050702123312.123456+01", "20050702113312.123456Z");
	recode("20050702123312.1234567+01", "20050702113312.123456Z");
	recode("20050702123312.12345678+01", "20050702113312.123456Z");
	recode("20050702123312.123456789+01", "20050702113312.123456Z");
	recode("20050702123312.000001+01", "20050702113312.000001Z");
	recode("20050702123312.0000001Z", "20050702123312Z");
	recode("20050702123312.0080010+1056", "20050702013712.008001Z");

	return 0;
}

/*
 * Dummy function.
 */

asn_enc_rval_t
OCTET_STRING_encode_der(asn_TYPE_descriptor_t *td, void *ptr, int tag_mode, ber_tlv_tag_t tag, asn_app_consume_bytes_f *cb, void *app_key) {
	asn_enc_rval_t erval;

	(void)td;
	(void)ptr;
	(void)tag_mode;
	(void)tag;
	(void)cb;
	(void)app_key;

	return erval;
}

asn_enc_rval_t
OCTET_STRING_encode_xer_utf8(asn_TYPE_descriptor_t *td, void *ptr, int ilevel, enum xer_encoder_flags_e flags, asn_app_consume_bytes_f *cb, void *app_key) {
	asn_enc_rval_t erval;

	(void)td;
	(void)ptr;
	(void)ilevel;
	(void)flags;
	(void)cb;
	(void)app_key;

	return erval;
}
