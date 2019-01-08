#from PIL import ImageStat
from base import utils, os_utils
from base.g import *
#from PIL import Image
#from PIL import ImageEnhance
#from tesserocr import PyTessBaseAPI, PSM
#from PyPDF2 import PdfFileMerger
#from PyPDF2 import PdfFileReader
#import numpy as np
#import PyPDF2
#import subprocess
#import imutils
#import cv2
import math
import os
import sys
import platform
from installer import core_install
from installer import dcheck
#from .dcheck import *
threshold = 200     #the average of the darkest values must be _below_ this to count (0 is darkest, 255 is lightest)
obviousness = 50    #how many of the darkest pixels to include (1 would mean a single dark pixel triggers it)

def rotate_image(image, angle):
    import numpy as np
    import cv2
    """
    Rotates an OpenCV 2 / NumPy image about it's centre by the given angle
    (in radians). The returned image will be large enough to hold the entire
    new image, with a black background
    """

    # Get the image size
    # No that's not an error - NumPy stores image matricies backwards
    image_size = (image.shape[1], image.shape[0])
    image_center = tuple(np.array(image_size) / 2)

    # Convert the OpenCV 3x2 rotation matrix to 3x3
    rot_mat = np.vstack(
        [cv2.getRotationMatrix2D(image_center, angle, 1.0), [0, 0, 1]]
    )

    rot_mat_notranslate = np.matrix(rot_mat[0:2, 0:2])

    # Shorthand for below calcs
    image_w2 = image_size[0] * 0.5
    image_h2 = image_size[1] * 0.5

    # Obtain the rotated coordinates of the image corners
    rotated_coords = [
        (np.array([-image_w2,  image_h2]) * rot_mat_notranslate).A[0],
        (np.array([ image_w2,  image_h2]) * rot_mat_notranslate).A[0],
        (np.array([-image_w2, -image_h2]) * rot_mat_notranslate).A[0],
        (np.array([ image_w2, -image_h2]) * rot_mat_notranslate).A[0]
    ]

    # Find the size of the new image
    x_coords = [pt[0] for pt in rotated_coords]
    x_pos = [x for x in x_coords if x > 0]
    x_neg = [x for x in x_coords if x < 0]

    y_coords = [pt[1] for pt in rotated_coords]
    y_pos = [y for y in y_coords if y > 0]
    y_neg = [y for y in y_coords if y < 0]

    right_bound = max(x_pos)
    left_bound = min(x_neg)
    top_bound = max(y_pos)
    bot_bound = min(y_neg)

    new_w = int(abs(right_bound - left_bound))
    new_h = int(abs(top_bound - bot_bound))

    # We require a translation matrix to keep the image centred
    trans_mat = np.matrix([
        [1, 0, int(new_w * 0.5 - image_w2)],
        [0, 1, int(new_h * 0.5 - image_h2)],
        [0, 0, 1]
    ])

    # Compute the tranform for the combined rotation and translation
    affine_mat = (np.matrix(trans_mat) * np.matrix(rot_mat))[0:2, :]

    # Apply the transform
    result = cv2.warpAffine(
        image,
        affine_mat,
        (new_w, new_h),
        flags=cv2.INTER_LINEAR
    )

    return result


def largest_rotated_rect(w, h, angle):
    """
    Given a rectangle of size wxh that has been rotated by 'angle' (in
    radians), computes the width and height of the largest possible
    axis-aligned rectangle within the rotated rectangle.

    Original JS code by 'Andri' and Magnus Hoff from Stack Overflow

    Converted to Python by Aaron Snoswell
    """

    quadrant = int(math.floor(angle / (math.pi / 2))) & 3
    sign_alpha = angle if ((quadrant & 1) == 0) else math.pi - angle
    alpha = (sign_alpha % math.pi + math.pi) % math.pi

    bb_w = w * math.cos(alpha) + h * math.sin(alpha)
    bb_h = w * math.sin(alpha) + h * math.cos(alpha)

    gamma = math.atan2(bb_w, bb_w) if (w < h) else math.atan2(bb_w, bb_w)

    delta = math.pi - alpha - gamma

    length = h if (w < h) else w

    d = length * math.cos(alpha)
    a = d * math.sin(alpha) / math.sin(delta)

    y = a * math.cos(gamma)
    x = y * math.tan(gamma)

    return (
        bb_w - 2 * x,
        bb_h - 2 * y
    )


def crop_around_center(image, width, height):
    """
    Given a NumPy / OpenCV 2 image, crops it to the given width and height,
    around it's centre point
    """

    image_size = (image.shape[1], image.shape[0])
    image_center = (int(image_size[0] * 0.5), int(image_size[1] * 0.5))

    if(width > image_size[0]):
        width = image_size[0]

    if(height > image_size[1]):
        height = image_size[1]

    x1 = int(image_center[0] - width * 0.5)
    x2 = int(image_center[0] + width * 0.5)
    y1 = int(image_center[1] - height * 0.5)
    y2 = int(image_center[1] + height * 0.5)

    return image[y1:y2, x1:x2]

def deskew(im):
    import numpy as np
    import cv2
    from PIL import Image
    median_flag = False
    '''image=np.array(im)
    image_height, image_width = image.shape[0:2]
    
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    edges = cv2.Canny(gray, 100, 100, apertureSize=3)
    lines = cv2.HoughLinesP(edges, 1, math.pi / 180.0, 100, minLineLength=100, maxLineGap=5)

    angles = []
    temp_angles = []

    for x1, y1, x2, y2 in lines[0]:
        angle = math.degrees(math.atan2(y2 - y1, x2 - x1))
        temp_angles.append(angle)
        angle = abs(angle)
        if angle != 0:
            angles.append(angle)

    median_angle = min(angles)
    for item in temp_angles:
        if median_angle == abs(item):
            median_angle = item
    #print(median_angle)
    if not (median_angle != 90 and median_angle != -90):
        return imbase/imageprocessing.py
    if (median_angle >= 45 or median_angle <= -45):
        return im'''
    '''# construct the argument parse and parse the arguments
    ap = argparse.ArgumentParser()
    ap.add_argument("-i", "--image", required=True,
	    help="path to input image file")
    args = vars(ap.parse_args())
    im=Image.open(args["image"])'''
    im = mixedfeed(im)
    #im.save("tmp.png")
    #im=Image.open("tmp.jpeg")
    # load the image from disk
    #image = cv2.imread(args["image"])
    #img = np.array(im)
    
    #image = cv2.imread('tmp.png')
    image = np.array(im)
    image_height, image_width = image.shape[0:2]
    # convert the image to grayscale and flip the foreground
    # and background to ensure foreground is now "white" and
    # the background is "black"
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    gray = cv2.bitwise_not(gray)

    # threshold the image, setting all foreground pixels to
    # 255 and all background pixels to 0
    thresh = cv2.threshold(gray, 0, 255,
	    cv2.THRESH_BINARY | cv2.THRESH_OTSU)[1]
    # grab the (x, y) coordinates of all pixel values that
    # are greater than zero, then use these coordinates to
    # compute a rotated bounding box that contains all
    # coordinates
    coords = np.column_stack(np.where(thresh > 0))
    angle = cv2.minAreaRect(coords)[-1]
    #print (angle)
    if angle < -45:
        angle = -(90 + angle)
    # otherwise, just take the inverse of the angle to make
    # it positive
    else:
        angle = -angle
    if abs(angle) == 90 or abs(angle) == 0:
        median_flag = True
        '''image=np.array(im)
        image_height, image_width = image.shape[0:2]'''
    
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        edges = cv2.Canny(gray, 100, 100, apertureSize=3)
        lines = cv2.HoughLinesP(edges, 1, math.pi / 180.0, 100, minLineLength=100, maxLineGap=5)

        angles = []
        temp_angles = []

        for x1, y1, x2, y2 in lines[0]:
            angle = math.degrees(math.atan2(y2 - y1, x2 - x1))
            temp_angles.append(angle)
            angle = abs(angle)
            if angle != 0:
                angles.append(angle)
        median_angle = min(angles)
        for item in temp_angles:
            if median_angle == abs(item):
                median_angle = item
        '''if median_angle > 0:
            median_angle = median_angle + 1
        else:
            median_angle = abs(median_angle) + 1
            median_angle = -(median_angle)'''
        if not (median_angle != 90 and median_angle != -90):
            return im
        if (median_angle >= 45 or median_angle <= -45):
            return im
    if median_flag:
        angle = median_angle
    image_orig = np.copy(image)
    image_rotated = rotate_image(image, angle)
    image_rotated_cropped = crop_around_center(
            image_rotated,
            *largest_rotated_rect(
                image_width,
                image_height,
                math.radians(angle)
            )
        )

    return Image.fromarray(image_rotated_cropped)


def blankpage(im,lineart_mode):
    from PIL import ImageStat
    ''' check for the blank page '''

    v = ImageStat.Stat(im).var
    #print(v)
    if lineart_mode and ( v[0] < 300 ):
        return True
    if ( ( v[0] < 300 ) and ( v[1] < 300 ) and ( v[2] < 300)) and (lineart_mode ==False):
        return True
    else:
        return False

def rotate_bound(image, angle):
    import cv2
    import numpy as np
    # grab the dimensions of the image and then determine the
    # center
    (h, w) = image.shape[:2]
    (cX, cY) = (w / 2, h / 2)

    # grab the rotation matrix (applying the negative of the
    # angle to rotate clockwise), then grab the sine and cosine
    # (i.e., the rotation components of the matrix)
    M = cv2.getRotationMatrix2D((cX, cY), -angle, 1.0)
    cos = np.abs(M[0, 0])
    sin = np.abs(M[0, 1])

    # compute the new bounding dimensions of the image
    nW = int((h * sin) + (w * cos))
    nH = int((h * cos) + (w * sin))

    # adjust the rotation matrix to take into account translation
    M[0, 2] += (nW / 2) - cX
    M[1, 2] += (nH / 2) - cY

    # perform the actual rotation and return the image
    return cv2.warpAffine(image, M, (nW, nH))
    
def orientangle(im):
    from tesserocr import PyTessBaseAPI, PSM
    with PyTessBaseAPI(psm=PSM.AUTO_OSD) as api:        
        try:        
            api.SetImage(im)
        except IOError:
            im = im.convert("RGB")
            api.SetImage(im)
        api.Recognize()

        it = api.AnalyseLayout()
        orientation, direction, order, deskew_angle = it.Orientation()
        return orientation

def autoorient(im, angle):
    from PIL import Image
    import numpy as np
    #import imutils
    orient_dict ={ 0 :0 ,1:270, 2:180, 3:90}
    rotated=rotate_bound(np.array(im),orient_dict[angle])
    return  Image.fromarray(rotated)


#Auto Crop Code
def initialcrop(img):    
    w, h = img.size
    return img.crop((10, 10, w-10, h-10))

def find_line(vals):
    #implement edge detection once, use many times 
    for i,tmp in enumerate(vals):
        tmp.sort()
        average = float(sum(tmp[:obviousness]))/len(tmp[:obviousness])
        if average <= threshold:
            return i
    return i    #i is left over from failed threshold finding, it is the bounds

def getbox(img):
    import numpy as np    
    #get the bounding box of the interesting part of a PIL image object
    #this is done by getting the darekest of the R, G or B value of each pixel
    #and finding were the edge gest dark/colored enough
    #returns a tuple of (left,upper,right,lower)

    width, height = img.size    #for making a 2d array
    retval = [0,0,width,height] #values will be disposed of, but this is a black image's box 

    pixels = list(img.getdata())
    vals = []                   #store the value of the darkest color
    for pixel in pixels:
        vals.append(min(pixel)) #the darkest of the R,G or B values

    #make 2d array
    pyPlatform = platform.python_version()
    num = pyPlatform.split('.')
    if num[0] >= '3':
        vals = np.array([vals[i * width:(i + 1) * width] for i in range(height)])
    else:
        vals = np.array([vals[i * width:(i + 1) * width] for i in xrange(height)])

    #start with upper bounds
    forupper = vals.copy()
    retval[1] = find_line(forupper)

    #next, do lower bounds
    #forlower = vals.copy()
    forlower = np.flipud(forupper)
    retval[3] = height - find_line(forlower)

    #left edge, same as before but roatate the data so left edge is top edge
    #forleft = vals.copy()
    forleft = np.swapaxes(forupper,0,1)
    retval[0] = find_line(forleft)

    #and right edge is bottom edge of rotated array
    #forright = vals.copy()
    forright = np.swapaxes(forupper,0,1)
    forright = np.flipud(forright)
    retval[2] = width - find_line(forright)

    if retval[0] >= retval[2] or retval[1] >= retval[3]:
        return None
    return tuple(retval)

def autocrop(im):
    from PIL import Image
    import numpy as np
    import cv2
    blurred = cv2.blur(np.array(im), (3,3))
    canny = auto_canny(blurred)
    
    ## find the non-zero min-max coords of canny
    pts = np.argwhere(canny>0)
    y1,x1 = pts.min(axis=0)
    y2,x2 = pts.max(axis=0)
    ## crop the region
    cropped = np.array(im)[y1:y2, x1:x2]
    img = initialcrop(Image.fromarray(cropped))
    box = getbox(img)
    result = img.crop(box)
    return result
    
def mixedfeed(im):
    from PIL import Image
    import numpy as np
    import cv2
    blurred = cv2.blur(np.array(im), (3,3))
    canny = cv2.Canny(blurred, 10, 200)
    ## find the non-zero min-max coords of canny
    pts = np.argwhere(canny>0)
    y1,x1 = pts.min(axis=0)
    y2,x2 = pts.max(axis=0)
    ## crop the region
    cropped = np.array(im)[0:y2, x1:x2]
    return Image.fromarray(cropped)
    
def generatePdfFile(adf_page_files,outputfile):
    from PyPDF2 import PdfFileMerger, PdfFileReader
    #temp = utils.createSequencedFilename("hpscanAuto1", ".pdf")
    #temp = 'temp.pdf'
    #output_file1 = utils.createSequencedFilename("hpscanmultifeed", ".pdf")
    merger = PdfFileMerger()
    for p in adf_page_files:
        '''image = Image.open(p)
        image = image.convert("RGB")
        image.save(temp)
        merger.append(open(temp,'rb'))
        cmd = 'rm -f ' + temp
        utils.run(cmd)'''
        merger.append(PdfFileReader(p), 'hpscan')
    '''with open(outputfile, 'wb') as fout:
        merger.write(fout)'''
    merger.write(outputfile)
    for p in adf_page_files:
        os.remove(p)
    return outputfile

def generatePdfFile_canvas(adf_page_files,outputfile,orient_list,brx,bry,tlx,tly,output_path):
    #print ("called canvas")
    from reportlab.pdfgen import canvas
    from PIL import Image
    '''adf_page_files2 = []
    for p in adf_page_files:
        output = utils.createSequencedFilename("hpscan", ".png", output_path)
        cmd = "convert %s %s" %(p,output)
        status = utils.run(cmd)
        #print (status[0])
        #print (status[1])
        if status[0] == -1:
            #print ("entered status -1")  
            log.error("Convert command not found.")
            sys.exit(6)
        adf_page_files2.append(output)
        #print(adf_page_files2)
    for p in adf_page_files:
        os.unlink(p) 
    adf_page_files = adf_page_files2
    #print (adf_page_files)
    #print (adf_page_files2)
    #print (outputfile)'''
    c = canvas.Canvas(outputfile, (brx/0.3528, bry/0.3528))
    i=0
    for p in adf_page_files:
       #log.info("Processing page %s..." % p)
       im = Image.open(p)

       try:
           if orient_list and (orient_list[i] == 1 or orient_list[i] == 3):
               c.setPageSize(((bry-tly)/0.3528, (brx-tlx)/0.3528))
               c.drawInlineImage(im, (tlx/0.3528), (tly/0.3528), ((bry-tly)/0.3528), ((brx-tlx)/0.3528))
           else:
               c.setPageSize(((brx-tlx)/0.3528, (bry-tly)/0.3528))
               c.drawInlineImage(im, (tlx/0.3528), (tly/0.3528), ((brx-tlx)/0.3528),((bry-tly)/0.3528))
       except NameError:
           #log.error("A problem has occurred with PDF generation. This is a known bug in ReportLab. Please update your install of ReportLab to version 2.0 or greater.")
           sys.exit(1)
       except AssertionError as e:
           log.error(e)
           if PY3:
               #log.note("You might be running an older version of reportlab. Please update to the latest version")
               #log.note("More information is available at http://hplipopensource.com/node/369")
               sys.exit(1)
       except Exception as e:
           #log.error(e)
           #log.note("Try Updating to reportlab version >= 3.2")
           sys.exit(1)

       c.showPage()
       os.unlink(p)
       i+=1

    #log.info("Saving to file %s" % outputfile)
    c.save()
    #del adf_page_files2[:]
    #del adf_page_files[:]
    return outputfile
    '''log.info("Viewing PDF file in %s" % pdf_viewer)
    cmd = "%s %s &" % (pdf_viewer, output)
    os_utils.execute(cmd)
    sys.exit(0)'''

def documentmerge(adf_page_files,ext,output_path):
    import numpy as np
    from PIL import Image
    from PyPDF2 import PdfFileMerger
    #print(output_type)
    #adf_page_files2 = []
    list_im = []
    i = 0
    if ext == ".pdf":
        merger = PdfFileMerger()
    '''if ext == ".pdf":
        merger = PdfFileMerger()
        for p in adf_page_files:
            output = utils.createSequencedFilename("hpscan", ".png", output_path)
            cmd = "convert %s %s" %(p,output)
            status = utils.run(cmd)
            #print (status[0])
            #print (status[1])
            if status[0] == -1:
                #print ("entered status -1")  
                log.error("Convert command not found.")
                sys.exit(6)
            adf_page_files2.append(output)
        #print(adf_page_files2)
        for p in adf_page_files:
            os.unlink(p) 
        adf_page_files = adf_page_files2
        #del adf_page_files2[:]
        #print(adf_page_files)'''      	
    while i < len(adf_page_files):       
        list_im = [adf_page_files[i], adf_page_files[i+1]]        	
        imgs    = [Image.open(y) for y in list_im ]
        # pick the image which is the smallest, and resize the others to match it (can be arbitrary image shape here)
        min_shape = sorted( [(np.sum(z.size), z.size ) for z in imgs])[0][1]
        imgs_comb = np.hstack( (np.asarray( w.resize(min_shape) ) for w in imgs ) )

        # save that beautiful picture
        imgs_comb = Image.fromarray( imgs_comb)
        for p in list_im:
            os.remove(p)        
        '''if ext == ".pdf":
            temp = 'temp.pdf'
            imgs_comb = imgs_comb.convert("RGB")
            imgs_comb.save( temp )
            merger.append(open(temp,'rb'))
            cmd = 'rm -f ' + temp
            utils.run(cmd)
        else:'''
        if ext == ".pdf":
            temp = 'temp.pdf'
            imgs_comb = imgs_comb.convert("RGB")
            imgs_comb.save( temp )
            merger.append(open(temp,'rb'))
            cmd = 'rm -f ' + temp
            utils.run(cmd)
            #adf_page_files2.append(temp)
            #merger = PdfFileMerger()
        else:
            temp = utils.createSequencedFilename("hpscandoc", ext, output_path)
            imgs_comb.save( temp )
        i = i + 2
    if ext == ".pdf":
        output = utils.createSequencedFilename("hpscandoc", ext, output_path)
        with open(output, 'wb') as fout:
            merger.write(fout)
        '''del adf_page_files2[:]
        del adf_page_files[:]'''
        return output
    else:
        return None
    
def auto_canny(image, sigma = 0.33):
    import numpy as np
    import cv2
    # compute the mediam of the single channel pixel intensities
    v = np.median(image)

    # apply automatic Canny edge detection using the computed median
    lower = int(max(0, (1.0 - sigma) * v))
    upper = int(min(255, (1.0 + sigma) *v))
    edged = cv2.Canny(image, lower, upper)

    # return edged image
    return edged


def crushed(im):
    from PIL import Image
    import numpy as np
    gray = im.convert('L')
    bw = np.asarray(gray).copy()
    bw[bw < 90] = 0    # Black
    bw[bw >= 180] = 255 # White
    return Image.fromarray(bw)
    #imfile.save("result_bw.png")
    #return imfile
    
def adjust_sharpness(im, factor):
    from PIL import ImageEnhance
    #image = Image.open(input_image)
    enhancer_object = ImageEnhance.Sharpness(im)
    out = enhancer_object.enhance(factor)
    return out
    
def adjust_contrast(im, factor):
    from PIL import ImageEnhance
    #image = Image.open(input_image)
    enhancer_object = ImageEnhance.Contrast(im)
    out = enhancer_object.enhance(factor)
    return out
    
def adjust_brightness(im, factor):
    from PIL import ImageEnhance
    #image = Image.open(input_image)
    enhancer_object = ImageEnhance.Brightness(im)
    out = enhancer_object.enhance(factor)
    return out

def adjust_color(im, factor):
    from PIL import ImageEnhance
    #image = Image.open(input_image)
    enhancer_object = ImageEnhance.Color(im)
    out = enhancer_object.enhance(factor)
    return out
   
def merge_PDF_viewer(output):
    pdf_viewer = ''
    pdf_viewer_list = ['kpdf', 'acroread', 'xpdf', 'evince', 'xdg-open']
    for v in pdf_viewer_list:
        vv = utils.which(v)
        if vv:
            pdf_viewer = os.path.join(vv, v)
            break
            #cmd = "%s %s &" % (pdf_viewer, output_pdf)
    cmd = pdf_viewer + "  " + output + " " + "&"
    #print(cmd)               
    os_utils.execute(cmd)

def check_pil():
    scanjet_flag = None
    try:
        import PIL	
    except ImportError as error:
        scanjet_flag=str(error)
		#.split(' ')[-1]
    except:
        scanjet_flag=str("Error occurred")
    return scanjet_flag

def check_numpy():
    scanjet_flag = None
    try:
        import numpy as np	
    except ImportError as error:
        scanjet_flag=str(error)
		#.split(' ')[-1]
    except:
        scanjet_flag=str("Error occurred")
    return scanjet_flag

def check_opencv():
    scanjet_flag = None
    try:
        import cv2	
    except ImportError as error:
        scanjet_flag=str(error)
		#.split(' ')[-1]
    except:
        scanjet_flag=str("Error occurred")
    return scanjet_flag

def check_tesserocr_imutils():
    scanjet_flag = None
    try:
        import tesserocr
        #import imutils	
    except ImportError as error:
        scanjet_flag=str(error)
		#.split(' ')[-1]
    except AttributeError as error:
        scanjet_flag=str(error)
    except:
        scanjet_flag=str("Error occurred")
    return scanjet_flag


def check_pypdf2():
    scanjet_flag = None
    try:
        import PyPDF2	
    except ImportError as error:
        scanjet_flag=str(error)
		#.split(' ')[-1]
    except:
        scanjet_flag=str("Error occurred")
    return scanjet_flag

def check_zbar():
    scanjet_flag = None
    pyPlatform = platform.python_version()
    num = pyPlatform.split('.')   
    if num[0] < '3':
        try:
            import zbar	
        except ImportError as error:
            scanjet_flag=str(error)
			#.split(' ')[-1]
        except:
            scanjet_flag=str("Error occurred")
        return scanjet_flag
    else:
        return 'zbar'

