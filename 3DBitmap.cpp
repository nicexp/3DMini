#include "3DBitmap.h"
#include "png/png.h"
#include "zlib/zlib.h"
#include "jpeg/jpeglib.h"

//创建位图
void CreateBitmap(BITMAP_IMG_PTR bitmap, int width, int height)
{
	bitmap->attr = 0;
	bitmap->state = 0;
	bitmap->width = width;
	bitmap->height = height;
#ifdef WINDDOW_BPP32
	bitmap->buffer = (UCHAR*)malloc(width * height*sizeof(UCHAR)* 4);
	bitmap->num_bytes = width * height*sizeof(UCHAR)* 4;
#else
	(*bitmap)->buffer = (UCHAR*)malloc(width * height*sizeof(UCHAR)* 2);
	(*bitmap)->num_bytes = width * height*sizeof(UCHAR)* 2;
#endif

	
	memset(bitmap->buffer, 0, bitmap->num_bytes);
}

//销毁位图数据
void DestroyBitmap(BITMAP_IMG_PTR bitmap)
{
	if (bitmap->buffer)
	{
		free(bitmap->buffer);
		bitmap->buffer = NULL;
	}
}

//加载位图数据
int Load_Bitmap_File(BITMAP_IMG_PTR texture, const char* filename)
{
	FILE* fp;
	BITMAP_FILE bp;
	BITMAP_FILE_PTR bitmap = &bp;
	UCHAR* tempbuffer = NULL;
	UINT red = 0, green = 0, blue = 0, alpha = 0;
	int num_pixels = 0;
	int index, srcidx;

	//读取文件
	fp = fopen(filename, "rb");
	if (!fp)
	{
		DEBUG_LOG("open file %s error", filename);
		return 0;
	}
	//读取文件头
	fread(&bitmap->bitmapfileheader, sizeof(BITMAPFILEHEADER), 1, fp);
	//是否为位图文件
	if (bitmap->bitmapfileheader.bfType != 0x4D42)
	{
		DEBUG_LOG("file is not bitmap！");
		fclose(fp);
		return 0;
	}
	//读取信息头部
	fread(&bitmap->bitmapinfoheader, sizeof(BITMAPINFOHEADER), 1, fp);
	num_pixels = bitmap->bitmapinfoheader.biWidth*bitmap->bitmapinfoheader.biHeight;
	DEBUG_LOG("bitcount:%d", bitmap->bitmapinfoheader.biBitCount);
	//为纹理创建位图
	CreateBitmap(texture, bitmap->bitmapinfoheader.biWidth, bitmap->bitmapinfoheader.biHeight);
	//判断是否读取调色板,8位模式下有调色板数据
	if (bitmap->bitmapinfoheader.biBitCount == 8)
	{
		fread(bitmap->palette, sizeof(PALETTEENTRY), 256, fp);
		//将BGR转换为RGB
		for (index = 0; index < 256; index++)
		{
			unsigned char tempcolor = bitmap->palette[index].peBlue;
			bitmap->palette[index].peBlue = bitmap->palette[index].peRed;
			bitmap->palette[index].peRed = tempcolor;
			bitmap->palette[index].peFlags = PC_NOCOLLAPSE;
		}
	}
	//读取位图颜色数据
	fseek(fp, -(int)(bitmap->bitmapinfoheader.biSizeImage), SEEK_END);
	if (bitmap->bitmapinfoheader.biBitCount == 16)
	{
		DEBUG_LOG("16 bit bitmap");
		if (!(tempbuffer = (UCHAR*)malloc(bitmap->bitmapinfoheader.biSizeImage)))
		{
			fclose(fp);
			return 0;
		}

		fread(bitmap->buffer, bitmap->bitmapinfoheader.biSizeImage, 1, fp);
		//将位图颜色数据写入纹理数据
		for (int index = 0; index < bitmap->bitmapinfoheader.biWidth * bitmap->bitmapinfoheader.biHeight; index++)
		{
			srcidx = num_pixels - 1 - index;
			UINT color = (UINT)((USHORT*)tempbuffer)[srcidx];
			_RGB565FROM16BIT(color, &red, &green, &blue);
#ifdef WINDDOW_BPP32
			((UINT*)texture->buffer)[index] = _RGBTOINT(red, green, blue);
#else
			((USHORT*)texture->buffer)[index] = _RGBTOINT(red, green, blue);
#endif
		}
		free(tempbuffer);
	}
	if (bitmap->bitmapinfoheader.biBitCount == 24)
	{
		DEBUG_LOG("24 bit bitmap");
		if (!(tempbuffer = (UCHAR*)malloc(bitmap->bitmapinfoheader.biSizeImage)))
		{
			fclose(fp);
			return 0;
		}

		fread(tempbuffer, bitmap->bitmapinfoheader.biSizeImage, 1, fp);

		for (index = 0; index < bitmap->bitmapinfoheader.biWidth*bitmap->bitmapinfoheader.biHeight; index++)
		{
			//读取像素
			srcidx = num_pixels - 1 - index;
			blue = (int)tempbuffer[srcidx * 3 + 0];
			green = (int)tempbuffer[srcidx * 3 + 1];
			red = (int)tempbuffer[srcidx * 3 + 2];
#ifdef WINDDOW_BPP32
			((UINT*)texture->buffer)[index] = _RGBTOINT(red, green, blue);
#else
			((USHORT*)texture->buffer)[index] = _RGBTOINT(red, green, blue);
#endif
		}
		free(tempbuffer);
	}
	if (bitmap->bitmapinfoheader.biBitCount == 32)
	{
		DEBUG_LOG("32 bit bitmap");
		if (!(tempbuffer = (UCHAR*)malloc(bitmap->bitmapinfoheader.biSizeImage)))
		{
			fclose(fp);
			return 0;
		}

		fread(tempbuffer, bitmap->bitmapinfoheader.biSizeImage, 1, fp);

		for (index = 0; index < bitmap->bitmapinfoheader.biWidth*bitmap->bitmapinfoheader.biHeight; index++)
		{
			//读取像素
			srcidx = num_pixels - 1 - index;
			blue = (int)tempbuffer[srcidx * 4 + 0];
			green = (int)tempbuffer[srcidx * 4 + 1];
			red = (int)tempbuffer[srcidx * 4 + 2];
			alpha = (int)tempbuffer[srcidx * 4 + 3];
#ifdef WINDDOW_BPP32
			((UINT*)texture->buffer)[index] = _RGBTOINT(red, green, blue);
#else
			((USHORT*)texture->buffer)[index] = _RGBTOINT(red, green, blue);
#endif
		}
		free(tempbuffer);
	}

	fclose(fp);
	return 1;
}

void Load_Png_File(BITMAP_IMG_PTR bitmap, const char* filename)
{
	png_image image;
	memset(&image, 0, sizeof(image));
	UINT red = 0, green = 0, blue = 0, alpha = 0;

	image.version = PNG_IMAGE_VERSION;
	if (png_image_begin_read_from_file(&image, filename) != 0);
	{
		png_bytep buffer;
		image.format = PNG_FORMAT_RGBA;
		
		buffer = (png_bytep)malloc(PNG_IMAGE_SIZE(image));

		if (buffer != NULL && (png_image_finish_read(&image, NULL, buffer, 0, NULL) != 0))
		{
			CreateBitmap(bitmap, image.width, image.height);

			for (int index = 0; index < image.width*image.height; index++)
			{
				//读取像素
				red = (int)buffer[index * 4 + 0];
				green = (int)buffer[index * 4 + 1];
				blue = (int)buffer[index * 4 + 2];
				alpha = (int)buffer[index * 4 + 3];
#ifdef WINDDOW_BPP32
				((UINT*)bitmap->buffer)[index] = _RGBTOINT(red, green, blue);
#else
				((USHORT*)bitmap->buffer)[index] = _RGBTOINT(red, green, blue);
#endif
			}
		}
		else
		{
			png_image_free(&image);
		}
	}
}


struct my_error_mgr {
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr *my_error_ptr;

METHODDEF(void)
my_error_exit(j_common_ptr cinfo)
{
	my_error_ptr myerr = (my_error_ptr)cinfo->err;
	(*cinfo->err->output_message) (cinfo);
	longjmp(myerr->setjmp_buffer, 1);
}

void Load_Jpeg_File(BITMAP_IMG_PTR bitmap, const char* filename)
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	FILE * infile;
	JSAMPARRAY buffer;
	int row_stride;
	UINT red = 0, green = 0, blue = 0, alpha = 0;

	if ((infile = fopen(filename, "rb")) == NULL) {
		DEBUG_LOG("can't open file %s", filename);
		return;
	}
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return;
	}
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	(void)jpeg_read_header(&cinfo, TRUE);
	(void)jpeg_start_decompress(&cinfo);
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

	CreateBitmap(bitmap, cinfo.image_width, cinfo.image_height);

	while (cinfo.output_scanline < cinfo.output_height) {
		(void)jpeg_read_scanlines(&cinfo, buffer, 1);
		for (int i = 0; i < bitmap->width; i++)
		{
			//读取像素
			red = (int)buffer[0][i * 3 + 0];
			green = (int)buffer[0][i * 3 + 1];
			blue = (int)buffer[0][i * 3 + 2];
#ifdef WINDDOW_BPP32
			((UINT*)bitmap->buffer)[bitmap->width * (cinfo.output_scanline - 1) + i] = _RGBTOINT(red, green, blue);
#else
			((USHORT*)bitmap->buffer)[bitmap->width * (cinfo.output_scanline - 1) + i] = _RGBTOINT(red, green, blue);
#endif
		}
	}

	(void)jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);
}

void ScrollBitmap(BITMAP_IMG_PTR image, int dx)
{
	BITMAP_IMG temp_image;

	if (!image || (dx == 0))
		return;
	
	int one_pixel_bype = 4;
	dx %= image->width;

	if (dx > 0) //右移
	{
		//临时位图
		CreateBitmap(&temp_image, dx, image->height);
		CopyBitmap(&temp_image, 0, 0,
			image, image->width - dx, 0,
			dx, image->height);

		UCHAR *source_ptr = image->buffer;
		int shift = one_pixel_bype * dx;

		for (int y = 0; y < image->height; y++)
		{
			//移动原位图，内存有重叠用memmove
			memmove(source_ptr + shift, source_ptr, (image->width - dx)*one_pixel_bype);
			//下一行
			source_ptr += (one_pixel_bype * image->width);
		}
		//重新拷贝到原位图
		CopyBitmap(image, 0, 0, &temp_image, 0, 0,
			dx, image->height);
	} 
	else //左移
	{
		dx = -dx;
		//临时位图
		CreateBitmap(&temp_image, dx, image->height);
		CopyBitmap(&temp_image, 0, 0,
			image, 0, 0,
			dx, image->height);

		UCHAR *source_ptr = image->buffer;
		int shift = one_pixel_bype * dx;

		for (int y = 0; y < image->height; y++)
		{
			//移动原位图，内存有重叠用memmove
			memmove(source_ptr, source_ptr + shift, (image->width - dx)*one_pixel_bype);
			//下一行
			source_ptr += (one_pixel_bype*image->width);
		}
		//重新拷贝到原位图
		CopyBitmap(image, image->width - dx, 0, &temp_image, 0, 0,
			dx, image->height);

	}

	DestroyBitmap(&temp_image);
}

//拷贝位图
void CopyBitmap(BITMAP_IMG_PTR dest_bitmap, int dest_x, int dest_y,
	BITMAP_IMG_PTR source_bitmap, int source_x, int source_y,
	int width, int height)
{
	if (!dest_bitmap || !source_bitmap)
		return;

	int bytes_per_pixel = 4;

	UCHAR *source_ptr = source_bitmap->buffer + (source_x + source_y * source_bitmap->width)*bytes_per_pixel;
	UCHAR *dest_ptr = dest_bitmap->buffer + (dest_x + dest_y * dest_bitmap->width)  *bytes_per_pixel;

	for (int y = 0; y < height; y++)
	{
		//拷贝一行
		memcpy(dest_ptr, source_ptr, bytes_per_pixel*width);
		//下一行
		source_ptr += (source_bitmap->width*bytes_per_pixel);
		dest_ptr += (dest_bitmap->width*bytes_per_pixel);
	}

	return;
}