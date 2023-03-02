
#include <stdio.h>
#include <abi_disp.h>
#include <abi_time.h>
#include <pax_gfx.h>

extern "C" int main() {
	printf("GAMINGGAMINGGAMINGGAMING\n");
	
	pax_buf_t *buf = (pax_buf_t *) disp_get_pax_buffer();
	// pax::Buffer cbuf(buf);
	
	uint16_t *fb = (uint16_t *) disp_get_framebuffer();
	
	pax_draw_rect(buf, 0xffff0000, 10, 10, 40, 40);
	
	disp_flush();
	
	while (1) {
		pax_background(buf, 0);
		pax_push_2d(buf);
		pax_apply_2d(buf, matrix_2d_translate(160, 120));
		pax_apply_2d(buf, matrix_2d_scale(50, 50));
		uint32_t now = millis();
		float angle = now % 3000 / 3000.0 * M_PI * 2.0;
		pax_apply_2d(buf, matrix_2d_rotate(angle));
		pax_draw_rect(buf, 0xffff0000, -1, -1, 2, 2);
		pax_pop_2d(buf);
		disp_flush();
	}
	
	while(1);
	return 0;
}
